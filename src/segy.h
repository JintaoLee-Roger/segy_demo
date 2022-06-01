/*
 * Copyright (c) 2022 Jintao Li. All rights reserved.
 * University of Science and Technology of China (USTC),
 * Computational and Interpretation Group (CIG).
 *
 * @author: Jintao Li
 *
 * @file: segy.h
 * @brief: statement of SegyFile class, which is used to convert
 *         a segy file to a binary file without any header information
 *         and padded with zeros if necessary.
 */

#ifndef SEGY_H_
#define SEGY_H_

#include "segy_header_info.h"

#include <fstream>
#include <climits>
#include <QString>
#include <vector>
#include <QObject>
#include <QProgressDialog>

class Segy : public QObject {
    Q_OBJECT
  public:
    Segy();
    ~Segy();

    // open a segy file by a name
    bool open_file(const char * infile);

    //textheader in EBCDIC format
    QString textHeader_EBCDIC();

    //textheader in ASCII format
    QString textHeader_ASCII();

    //textheader in guess format
    QString textHeader_default();

    //binary header, format: (loc) discription: value
    QString binaryHeader_QString();

    //trace header in t_num_th trace, format: (loc) discription: value
    QString traceHeader_QString(int64_t t_num);

    // the guessed location
    QString get_default_inline_loc();
    QString get_default_xline_loc();

    // the important parameters of segy file
    QString scan();

    // change the location
    void set_location(int inline_loc, int xline_loc, int x_loc, int y_loc);

    // convert
    bool toDat(const QString outfile, QProgressDialog & qpd);

  signals:
    // emit a number to show the process
    void scan_process(int proc);
    void to_dat_process(int proc);

  private:
    std::fstream in_; // segy file
    std::fstream out_; // .dat file
    bool is_ebcdic;
    char text_header[3201];

    // nt, dt, dformat, ntrace, in_loc, xl_loc
    // x_loc, y_loc, in_max, in_min, xl_max, xl_min
    // x_max, x_min, y_max, y_min, ni, nx, total_trace
    std::map<const char *, int64_t> bkeys = {}; // keys
    double in_interval = 0;
    double xl_interval = 0;

    // read 3200 byte
    void getTextHeader();

    // guess text header format
    bool isTextInEBCDICFormat(const char * text);

    // convert EBCDIC char to ASCII char
    char getASCIIFromEBCDIC(char c);

    // get binary or trace header key value in a key location
    // return: a key value
    // params:
    //      loc: location of a key
    //      is_binaryheader: binaryheader key or traceheader key
    //      trace: trace index, >= 1,
    //             this param will be ignore when is_binaryheader is true
    int64_t getValueFromLocation(const int loc, bool is_binaryheader = true, int64_t trace = 1);

    // guess inline and crossline number's location
    void guessLoc();

    // scan the range of inline, crossline and their interval
    void scan_();
    void clc_trace_interval();

    float ibm_to_ieee(float value, bool is_big_endian_input);
    void readOneTrace(std::vector<float>& trace, int64_t idx_trace);
    void writeOneTrace(const std::vector<float>& trace);

    template <typename T>
    T swap_endian(T u);

};

template <typename T>
T Segy::swap_endian(T u) {
    static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

    union {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}


#endif
