#
# Copyright (c) 2015 Regents of the SIGNET lab, University of Padova.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University of Padova (SIGNET lab) nor the 
#    names of its contributors may be used to endorse or promote products 
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


AC_INIT(M2Mo,1.0)
AM_INIT_AUTOMAKE 
# AC_PROG_RANLIB
AC_LANG(C++)
AC_PROG_CXX
LT_INIT
AC_CONFIG_MACRO_DIR([m4])

AC_DEFINE(CPP_NAMESPACE,std) 

AC_CHECK_LIB(pthread, pthread_create, [LIBS="$LIBS -lpthread"]) 

AC_CONFIG_FILES([
    Makefile
    uwmphy_modem/Makefile
    ms2c_evologics/Makefile
    ms2c_evologics_ll/Makefile
    pandaboard/Makefile
    utilities/Makefile
    utilities_ll/Makefile
    executable/base_sample/Makefile
    executable/burst/Makefile
    executable/burst_lowlevel/Makefile
    executable/gpIO/Makefile
    executable/m2mo/Makefile
    executable/multi-modal/Makefile
    executable/source_localization/Makefile
    ])

AC_OUTPUT 
