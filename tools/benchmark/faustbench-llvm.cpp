
/************************************************************************
    FAUST Architecture File
    Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it 
    and/or modify it under the terms of the GNU General Public License 
    as published by the Free Software Foundation; either version 3 of 
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License 
    along with this program; If not, see <http://www.gnu.org/licenses/>.

    EXCEPTION : As a special exception, you may create a larger work 
    that contains this FAUST architecture section and distribute  
    that work under terms of your choice, so long as this FAUST 
    architecture section is not modified. 

 ************************************************************************/

#include "faust/dsp/dsp-optimizer.h"
#include "faust/misc.h"

using namespace std;

template <typename T>
static void bench(dsp_optimizer<T> optimizer, const string& name)
{
    double value;
    vector<string> options = optimizer.findOptimizedParameters(value);
    cout << "Best value is for '" << name << "' is : " << value << " with ";
    for (int i = 0; i < options.size(); i++) {
        cout << options[i] << " ";
    }
    cout << endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout << "faustbench-llvm [-run num] [additional Faust options (-vec -vs 8...)] <foo.dsp>" << endl;
        return 0;
    }
    
    int index = 1;
    bool is_double = isopt(argv, "-double");
    int run = lopt(argv, "-run", 1);
    int buffer_size = 1024;
    
    cout << "Libfaust version : " << getCLibFaustVersion () << std::endl;
    
    int argc1 = 0;
    const char* argv1[64];
    
    cout << "Compiled with additional options : ";
    for (int i = 1; i < argc-1; i++) {
        if (string(argv[i]) == "-run") {
            i++;
            continue;
        }
        argv1[argc1++] = argv[i];
        cout << argv[i] << " ";
    }
    cout << endl;
      
    // Add library
    argv1[argc1++] = "-I";
    argv1[argc1++] = "/usr/local/share/faust";
 
    argv1[argc1] = 0;  // NULL terminated argv
    
    try {
        if (is_double) {
            bench(dsp_optimizer<double>(argv[argc-1], argc1, argv1, "", buffer_size, run, -1), argv[1]);
        } else {
            bench(dsp_optimizer<float>(argv[argc-1], argc1, argv1, "", buffer_size, run, -1), argv[1]);
        }
    } catch (...) {}
    
  	return 0;
}