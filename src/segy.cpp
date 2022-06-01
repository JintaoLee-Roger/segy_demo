/*
 * Copyright (c) 2022 Jintao Li. All rights reserved.
 * University of Science and Technology of China (USTC),
 * Computational and Interpretation Group (CIG).
 *
 * @author: Jintao Li
 *
 * @file: segy.cpp
 * @brief: definition of SegyFile class
 */

#include "segy.h"
#include <iostream>
#include <cmath>

Segy::Segy() {

}

Segy::~Segy() {
    if (in_) {
        in_.close();
    }
    if (out_) {
        out_.close();
    }
}


// public

bool Segy::open_file(const char * infile){
    if (in_) {
        in_.close();
        bkeys.clear();
        in_interval = 0;
        xl_interval = 0;
    }
    in_.open(infile, std::ios::in | std::ios::binary);
    if (! in_) {
        return false;
    }

    getTextHeader();
    is_ebcdic = isTextInEBCDICFormat(&text_header[0]);

    bkeys["nt"] = getValueFromLocation(21, true);
    bkeys["dt"] = getValueFromLocation(17, true);
    bkeys["dformat"] = getValueFromLocation(25, true);
    bkeys["x_loc"] = 73;
    bkeys["y_loc"] = 77;

    in_.seekg(0, std::ios::end);
    uintmax_t flength = in_.tellg();
    bkeys["ntrace"] = (flength - 3600) / (240 + bkeys["nt"] * 4);

    guessLoc();

    return true;
}

QString Segy::textHeader_default() {
    if (is_ebcdic) {
        return textHeader_EBCDIC();
    }
    else {
        return textHeader_ASCII();
    }
}

QString Segy::textHeader_ASCII() {
    char line[81] = {0};
    QString out = "Segy File Text Header, Text Format: ASCII \n";

    for (int i = 0, j = 0; i < 3200; ++i, ++j) {
        j = j % 80;
        line[j] = text_header[i];

        if ((i + 1) % 80 == 0) {
            out += QString::fromLocal8Bit(line);
            out += "\n";
        }
    }

    return out;
}

QString Segy::textHeader_EBCDIC() {
    char line[81] = {0};
    QString out = "Segy File Text Header, Text Format: EBCDIC \n\n";

    for (int i = 0, j = 0; i < 3200; ++i, ++j) {
        j = j % 80;
        line[j] = getASCIIFromEBCDIC(text_header[i]);

        if ((i + 1) % 80 == 0) {
            out += QString::fromLocal8Bit(line);
            out += "\n";
        }
    }

    return out;
}


QString Segy::binaryHeader_QString() {
    QString out = "Segy File Binary Header: \n\n";

    for (auto &loc_key : keysmapBH) {
        out += ("(" + QString::number(loc_key.first) + ")\t");
        out += loc_key.second;
        out += ":    ";
        out += QString::number(getValueFromLocation(loc_key.first, true));
        out += "\n";
    }

    return out;
}

QString Segy::traceHeader_QString(int64_t t_num) {
    QString out = "Segy File Text Header, trace number: " + QString::number(t_num) + "\n\n";

    if (t_num <=0 ) {
        out += "Error!\nTrace Number must lager than 0, i.e. >= 1\n";
        return out;
    }

    for (auto &loc_key : keysmapTH) {
        out += ("(" + QString::number(loc_key.first) + ")\t");
        out += loc_key.second;
        out += ":    ";
        out += QString::number(getValueFromLocation(loc_key.first, false, t_num));
        out += "\n";
    }

    return out;
}


QString Segy::get_default_inline_loc() {
    return QString::number(bkeys["in_loc"]);
}

QString Segy::get_default_xline_loc() {
    return QString::number(bkeys["xl_loc"]);
}



void Segy::set_location(int inline_loc, int xline_loc, int x_loc, int y_loc) {
    if (inline_loc != bkeys["in_loc"] ||
        xline_loc != bkeys["xl_loc"] ||
        x_loc != bkeys["x_loc"] ||
        y_loc != bkeys["y_loc"]) {
        bkeys["in_loc"] = inline_loc;
        bkeys["xl_loc"] = xline_loc;
        bkeys["x_loc"] = x_loc;
        bkeys["y_loc"] = y_loc;
        scan_();
    }
}


QString Segy::scan(){
    if (bkeys.find("in_max") == bkeys.end()) {
        scan_();
    }
    QString out = "Segy data information: \n\n";

    out += ("nt (N samples per trace): " + QString::number(bkeys["nt"]) + "\n");
    out += ("dt (Sample interval): " + QString::number(bkeys["dt"]) + "\n");
    out += ("dformat (Data sample format code, 1-IBM, 5-IEEE): " + QString::number(bkeys["dformat"]) + "\n");
    out += ("ntrace (Number of tarces): " + QString::number(bkeys["ntrace"]) + "\n\n");

    out += ("Inline location: " + QString::number(bkeys["in_loc"]) + "\n");
    out += ("crossline location: " + QString::number(bkeys["xl_loc"]) + "\n");

    out += ("X location: " + QString::number(bkeys["x_loc"]) + "\n");
    out += ("Y location: " + QString::number(bkeys["y_loc"]) + "\n");

    out += "Scalar to coordinates: ";
    out += QString::number(getValueFromLocation(71, false, 1)) + "\n";

    out += ("Inline interval: " + QString::number(in_interval) + "\n");
    out += ("Crossline interval: " + QString::number(xl_interval) + "\n");

    out += ("Inline range: " + QString::number(bkeys["in_min"]) + " - " + QString::number(bkeys["in_max"]) + "\n");
    out += ("Crossline range: " + QString::number(bkeys["xl_min"]) + " - " + QString::number(bkeys["xl_max"]) + "\n\n");

    out += ("Data shape (ni, nx, nt): " + QString::number(bkeys["ni"]) + ", " + QString::number(bkeys["nx"]) + ", " + QString::number(bkeys["nt"]) + "\n");

    return out;
}


bool Segy::toDat(const QString outfile, QProgressDialog & qpd) {
    if (bkeys.find("in_max") == bkeys.end()) {
        scan_();
    }

    out_.open(outfile.toStdString(), std::ios::out | std::ios::binary);
    if (!out_) {
//        std::cout << "Open file: " << outfile << " failure" << std::endl;
        exit(1);
    }

    std::vector<float> trace(bkeys["nt"], 0);

    int64_t idx = 1;
    size_t inl = getValueFromLocation(bkeys["in_loc"], false, 1);
    size_t xl = getValueFromLocation(bkeys["xl_loc"], false, 1);
    for (int i = bkeys["in_min"]; i <= bkeys["in_max"]; ++i) {
        for (int j = bkeys["xl_min"]; j <= bkeys["xl_max"]; ++j) {
            if (inl == i && xl == j) {
                readOneTrace(trace, idx);
                writeOneTrace(trace);
                ++idx;
                inl = getValueFromLocation(bkeys["in_loc"], false, idx);
                xl = getValueFromLocation(bkeys["xl_loc"], false, idx);
            }
            else {
                std::fill(trace.begin(), trace.end(), 0);
                writeOneTrace(trace);
            }

            // emit a signal, to show the process
            int64_t idx_total = (i - bkeys["in_min"]) * bkeys["nx"] + (j - bkeys["xl_min"]);
            if (0 == idx_total % (bkeys["total_trace"] / 100)) {
                qpd.setValue(idx_total * 100 / bkeys["total_trace"] + 1);

                if(qpd.wasCanceled()){
                    out_.close();
                    return false;
                }
            }
        }
    }

    out_.close();

    return true;
}


// private

void Segy::getTextHeader() {
    in_.seekg(0, std::ios::beg);
    in_.read(&text_header[0], 3200);
}

bool Segy::isTextInEBCDICFormat(const char* text) {
    int alnumASCII = 0;
    for (size_t i = 0; i < 3200; i++) {
        if (std::isalnum(text[i])) alnumASCII++;
    }

    int alnumEBCDIC = 0;
    for (size_t i = 0; i < 3200; i++) {
        if (std::isalnum(getASCIIFromEBCDIC(text[i]))) alnumEBCDIC++;
    }

    if (alnumASCII > alnumEBCDIC) return false;
    return true;
}

char Segy::getASCIIFromEBCDIC(char c) {
    if (kEBCDICtoASCIImap.find(c) != kEBCDICtoASCIImap.end())
        return kEBCDICtoASCIImap.at(c);
    return ' ';
}

int64_t Segy::getValueFromLocation(const int loc, bool is_binaryheader, int64_t trace) {
    int64_t absoluteLoc = (is_binaryheader ? loc + 3200 : 3600 + (trace-1)*(240+bkeys["nt"]*4) + loc); // Note
    in_.seekg(absoluteLoc-1, std::ios::beg);
    if ( ( is_binaryheader && ( keys4bBH.find(loc) != keys4bBH.end() ) ) ||
        ( !is_binaryheader && ( keys4bTH.find(loc) != keys4bTH.end() ) ) ){
        int32_t value = 0;
        in_.read((char *)&value, 4);
        return swap_endian(value);
    }
    else if ( is_binaryheader && ( keys8bBH.find(loc) != keys8bBH.end() ) ){
        int64_t value = 0;
        in_.read((char *)&value, 8);
        return swap_endian(value);
    }
    else {
        int16_t value = 0;
        in_.read((char *)&value, 2);
        return swap_endian(value);
    }
}

void Segy::guessLoc() {
//     size_t inline_loc[3] = {189, 9, 5};
//     size_t xline_loc[3] = {193, 21, 17};
    size_t inline_loc[3] = {5, 9, 189};
    size_t xline_loc[3] = {17, 21, 193};
    uintmax_t start1 = 1;
    uintmax_t start3 = bkeys["ntrace"]-1;

    for (size_t i = 0; i < 3; ++i) {
        int inl1 = getValueFromLocation(inline_loc[i], false, start1);
        int inl2 = getValueFromLocation(inline_loc[i], false, start3);

        if (inl1 > 0 && inl2 > 0 && inl1 != inl2 && (inl2 - inl1) < 10000) {
            bkeys["in_loc"] = inline_loc[i];
            break;
        }
    }
    for (size_t i = 0; i < 3; ++i) {
        int xl1 = getValueFromLocation(xline_loc[i], false, start1);
        int xl2 = getValueFromLocation(xline_loc[i], false, start3);

        if (xl1 > 0 && xl2 > 0 && xl1 != xl2 && (xl2 - xl1) < 10000) {
            bkeys["xl_loc"] = xline_loc[i];
            break;
        }
    }
}

void Segy::scan_() { // TODO, inline, crossline interval
    // inline range
    int inmin = getValueFromLocation(bkeys["in_loc"], false, 1);
    int inmax = getValueFromLocation(bkeys["in_loc"], false, bkeys["ntrace"]);
    bkeys["in_max"] = inmax;
    bkeys["in_min"] = inmin;
    bkeys["ni"] = inmax - inmin + 1;

    // crossline range
    int temp = getValueFromLocation(bkeys["xl_loc"], false, 1);
    int xlmax = temp;
    int xlmin = temp;

    for (int64_t i = 2; i < bkeys["ntrace"]+1; ++i) {
        temp = getValueFromLocation(bkeys["xl_loc"], false, i);
        if (temp > xlmax) xlmax = temp;
        if (temp < xlmin) xlmin = temp;

        // emit a signal, to show the process
        if (0 == i % (bkeys["ntrace"] / 10)) {
            emit scan_process(i * 10 / bkeys["ntrace"]);
        }
    }

    bkeys["xl_max"] = xlmax;
    bkeys["xl_min"] = xlmin;
    bkeys["nx"] = xlmax - xlmin + 1;

    // total trace
    bkeys["total_trace"] = bkeys["ni"] * bkeys["nx"];

    // inline, crossline intervals
    clc_trace_interval();
}

void Segy::clc_trace_interval() {
    // nx must larger than 11, ntrace must larger than 5*nx+1
    int64_t x1 = getValueFromLocation(bkeys["x_loc"], false, 1);
    int64_t y1 = getValueFromLocation(bkeys["y_loc"], false, 1);
    int in1 = getValueFromLocation(bkeys["in_loc"], false, 1);
    int xl1 = getValueFromLocation(bkeys["xl_loc"], false, 1);

    int in2, xl2;
    int64_t x2;
    int64_t y2;

    x2 = getValueFromLocation(bkeys["x_loc"], false, 11);
    y2 = getValueFromLocation(bkeys["y_loc"], false, 11);
    xl_interval = std::sqrt(std::pow(x2-x1, 2) + std::pow(y2-y1, 2)) / 10.0;

    in2 = getValueFromLocation(bkeys["in_loc"], false, 5*bkeys["nx"]+1);
    xl2 = getValueFromLocation(bkeys["xl_loc"], false, 5*bkeys["nx"]+1);
    x2 = getValueFromLocation(bkeys["x_loc"], false, 5*bkeys["nx"]+1);
    y2 = getValueFromLocation(bkeys["y_loc"], false, 5*bkeys["nx"]+1);

    in_interval = std::sqrt(std::pow(y2-y1, 2) + std::pow(x2-x1, 2) -
                            std::pow((xl2-xl1)*xl_interval, 2)) / (in2-in1);

}

void Segy::readOneTrace(std::vector<float>& trace, int64_t idx_trace) {
    uintmax_t absolute_loc = 3600 + (idx_trace-1)*(240+bkeys["nt"]*4) + 240;

    in_.seekg(absolute_loc, std::ios::beg);
    in_.read((char*)&trace[0], bkeys["nt"] * 4);

    for (auto& i : trace) {
        if (1 == bkeys["dformat"]) {
            i = ibm_to_ieee(i, true);
        }
        else {
            i = swap_endian(i);
        }
    }
}

void Segy::writeOneTrace(const std::vector<float>& trace) {
    out_.write((char*)&trace[0], bkeys["nt"] * 4);
}

float Segy::ibm_to_ieee(float value, bool is_big_endian_input) {
    if (is_big_endian_input) {
        value = swap_endian<float>(value);
    }

    int32_t* int_addr = reinterpret_cast<int32_t*>(&value);
    int32_t int_val = *int_addr;

    int32_t sign = int_val >> 31;
    int32_t fraction = int_val & 0x00ffffff;

    if (fraction == 0) {
        return sign ? -0.0f : 0.0f;
    }

    // Convert exponent to be of base 2 and remove IBM exponent bias.
    int32_t exponent = ((int_val & 0x7f000000) >> 22) - 256;

    // Drop the last bit since we can store only 23 bits in IEEE.
    fraction >>= 1;

    // Normalize such that the implicit leading bit of the fraction is 1.
    while (fraction && (fraction & 0x00800000) == 0) {
    fraction <<= 1;
    --exponent;
    }

    // Drop the implicit leading bit.
    fraction &= 0x007fffff;

    // Add IEEE bias to the exponent.
    exponent += 127;

    // Handle overflow.
    if (exponent >= 255) {
        return (sign ? -std::numeric_limits<float>::max()
                    : std::numeric_limits<float>::max());
    }

    int32_t ieee_value;

    // Handle underflow.
    if (exponent <= 0)
        ieee_value = (sign << 31) | fraction;
    else
        ieee_value = (sign << 31) | (exponent << 23) | fraction;

    float* float_addr = reinterpret_cast<float*>(&ieee_value);
    return *float_addr;
}





