/*
 Copyright 2016 Jussi Pakkanen

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#pragma once

#include<cassert>

struct try_result {
    bool was_num;
    long value;
    bool has_next;
    char next_char;
};

template<class T>
try_result tryint(T &b, T&e, bool lower) {
    try_result r;
    r.was_num = false;
    r.value = 0;
    r.has_next = true;
    assert(b != e);
    while(b != e) {
        if (lower)
            r.next_char = tolower(*b++);
        else
            r.next_char = *b++;
        if (r.next_char >= '0' && r.next_char <= '9') {
            r.was_num = true;
            r.value = 10*r.value + (r.next_char - '0');
        } else {
            return r;
        }
    }
    r.has_next = false;
    return r;
}

/*
 * Evaluate natural order using only forward iterators. Natural order means
 * that embedded numbers are sorted according to numerical value instead of
 * ASCIIbetical order. Returns -1, 0 or 1 depending on whether the first
 * string is less than, equal or greater than the second, respectively.
 */

//extern "C" void dumpStr(const char*);
template<class T>
int natural_compare(T str1_begin, T str1_end, T str2_begin, T str2_end, bool lower = false) {
    while(true) {
        auto end1 = str1_begin == str1_end;
        auto end2 = str2_begin == str2_end;
        if(end1) {
            if(!end2) {
                return -1;
            }
            return 0;
        }
        if(end2) {
            return 1;
        }
/*        
        dumpStr(str1_begin);
        dumpStr(" vs. ");
        dumpStr(str2_begin);
        dumpStr("\n");
//*/        
        auto res1 = tryint(str1_begin, str1_end, lower);
        auto res2 = tryint(str2_begin, str2_end, lower);
        if(res1.was_num) {
            if(res2.was_num) {
                if(res1.value < res2.value) {
                    return -1;
                }
                if(res1.value > res2.value) {
                    return 1;
                }
            } else {
                // str1 was int but str2 was not.
                return res2.next_char > '0' ? -1 : 1;
            }
        } else if(res2.was_num) {
            assert(!res1.was_num);
            // str1 was not int but str2 was;
            return res1.next_char < '0' ? -1 : 1;
        } else {
            assert(!res1.was_num);
            assert(!res2.was_num);
        }

        bool doContinue = false;
        if (!res1.has_next) // one string is exhausted...
        {
            if (res2.has_next) str2_begin--; // put next_char back
            doContinue = true;
        }

        if (!res2.has_next) // the other string is exhausted...
        {
            if (res1.has_next) str1_begin--; // put next_char back
            doContinue = true;
        }
        
        if (doContinue) // ...don't compare next_char!
            continue;                         

        if(res1.next_char < res2.next_char) {
            return -1;
        }
        if(res1.next_char > res2.next_char) {
            return 1;
        }
    }
}

/*
 * Overload for types that have .begin() and .end().
 */
template<class T1, class T2>
int natural_compare(T1 &str1, T2 &str2, bool lower = false) {
    return natural_compare(str1.begin(), str1.end(), str2.begin(), str2.end(), lower);
}

/*
 * Comparator for use in std::sort.
 */
template<class T>
int natural_less(T &s1, T &s2, bool lower = false) {
    return natural_compare(s1, s2, lower) < 0;
};
