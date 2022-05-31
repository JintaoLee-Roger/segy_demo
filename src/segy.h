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
//#include <QObject>

class Segy {
  public:
    Segy();
    ~Segy();

    bool open_file(const char * infile);
    QString textHeader_EBCDIC();
    QString textHeader_ASCII();
    QString textHeader_default();
    QString binaryHeader_QString();
    QString traceHeader_QString(int64_t t_num);
    QString get_default_inline_loc();
    QString get_default_xline_loc();
    QString scan();

    void set_location(int inline_loc, int xline_loc, int x_loc, int y_loc);

    bool toDat(const QString outfile);

  private:
    std::fstream in_;
    std::fstream out_;
    bool is_ebcdic;
    char text_header[3201];
    std::map<const char *, int64_t> bkeys = {}; // nt, dt, dformat, ntrace, in_loc, x_loc

    void getTextHeader();
    bool isTextInEBCDICFormat(const char * text);
    char getASCIIFromEBCDIC(char c);

    int64_t getValueFromLocation(const int loc, bool is_binaryheader = true, int64_t trace = 1);

    void guessLoc();

    void scan_();

    float ibm_to_ieee(float value, bool is_big_endian_input);
    void readOneTrace(std::vector<float>& trace, int64_t idx_trace);
    void writeOneTrace(const std::vector<float>& trace);

    template <typename T>
    T swap_endian(T u);

  signals:
    void scan_process(int proc);
    void to_dat_process(int proc);

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
