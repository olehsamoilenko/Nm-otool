#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

if [[ $1 = "" ]]; then
	otool_correct=(
		fat/MachO-iOS-armv7-armv7s-arm64-Helloworld
		fat_lib/ssh-keychain.dylib
		32/MachO-OSX-x86-ls
		32/MachO-iOS-armv7s-Helloworld
		fat_lib/libsystemstats.dylib
		lib_stat/libft32.a
		64/64_bundle
		lib_stat/libmalloc_test.a
		lib_stat/libftprintf32.a
		lib_stat/libftprintf.a
		64/64_lib_dynamic_hard
		64/64_lib_dynamic_medium
		fat_lib/libAccountPolicyTranslation.dylib
		fat/appsleepd
		64/MachO-OSX-x64-ls
		test_moins_facile
		test_facile
		64/64_exe_easy
		64/64_exe_hard
		64/global
		64/64_exe_medium
		fat_lib/PN548_API.dylib
		32/MachO-iOS-arm1176JZFS-bash
		fat_lib/libstdc++.dylib
		fat_lib/libstdc++.6.dylib
		fat_lib/PN548_HAL_OSX.dylib
		fat_lib/libstdc++.6.0.9.dylib
		32/32_exe_hard
		fat_lib/liblapack.dylib
		fat_lib/libcblas.dylib
		fat_lib/libkrb5support.dylib
		fat_lib/libarchive.dylib
		fat_lib/libate.dylib
		fat_lib/libblas.dylib
		fat_lib/libc++.1.dylib
		fat_lib/libpmenergy.dylib
		fat_lib/libobjc.dylib
		fat_lib/libobjc.A.dylib
		fat_lib/libmx.A.dylib
		fat_lib/libmenu.dylib
		fat_lib/libc++.dylib
		fat_lib/libclapack.dylib
		fat_lib/libcom_err.dylib
		fat_lib/libcompression.dylib
		fat_lib/libcoretls.dylib
		fat_lib/libdes425.dylib
		fat_lib/libdns_services.dylib
		fat_lib/libextension.dylib
		fat_lib/libf77lapack.dylib
		fat_lib/libform.5.4.dylib
		fat_lib/libFosl_dynamic.dylib
		fat_lib/libft_malloc_x86_64_Darwin.so
		fat_lib/libgssapi_krb5.dylib
		fat_lib/libhunspell-1.2.dylib
		fat_lib/libicucore.dylib
		fat_lib/libinfo.dylib
		fat_lib/libk5crypto.dylib
		fat_lib/libkrb4.dylib
		fat_lib/libkrb524.dylib
		fat_lib/libkrb5.dylib
		fat_lib/libz.dylib
		fat_lib/libz.1.dylib
		fat_lib/libsqlite3.dylib
		fat_lib/libsqlite3.0.dylib
		fat_lib/libpmsample.dylib
		fat_lib/libform.dylib
		fat/fat_hard
		fat/MachO-iOS-armv7-armv7s-arm64-Helloworld
		lib_stat/lib_long_name.a
		lib_stat/libft_static.a
	)
	nm_correct=(
		fat_lib/ssh-keychain.dylib
		fat_lib/PN548_OSX.dylib
		32/MachO-OSX-x86-ls
		32/MachO-iOS-armv7s-Helloworld
		fat_lib/libsystemstats.dylib
		lib_stat/libft32.a
		64/64_bundle
		lib_stat/libmalloc_test.a
		lib_stat/libftprintf32.a
		lib_stat/libftprintf.a
		64/64_lib_dynamic_hard
		64/64_lib_dynamic_medium
		fat_lib/libAccountPolicyTranslation.dylib
		fat/appsleepd
		64/MachO-OSX-x64-ls
		test_moins_facile
		test_facile
		64/64_exe_easy
		64/64_exe_hard
		64/global
		64/64_exe_medium
		fat_lib/PN548_API.dylib
		32/MachO-iOS-arm1176JZFS-bash
		fat_lib/libstdc++.dylib
		fat_lib/libstdc++.6.dylib
		fat_lib/PN548_HAL_OSX.dylib
		fat_lib/libstdc++.6.0.9.dylib
		32/32_exe_hard
		fat_lib/liblapack.dylib
		fat_lib/libcblas.dylib
		fat_lib/libkrb5support.dylib
		fat_lib/libarchive.dylib
		fat_lib/libate.dylib
		fat_lib/libblas.dylib
		fat_lib/libc++.1.dylib
		fat_lib/libpmenergy.dylib
		fat_lib/libobjc.dylib
		fat_lib/libobjc.A.dylib
		fat_lib/libmx.A.dylib
		fat_lib/libmenu.dylib
		fat_lib/libc++.dylib
		fat_lib/libclapack.dylib
		fat_lib/libcom_err.dylib
		fat_lib/libcompression.dylib
		fat_lib/libcoretls.dylib
		fat_lib/libdes425.dylib
		fat_lib/libdns_services.dylib
		fat_lib/libextension.dylib
		fat_lib/libf77lapack.dylib
		fat_lib/libform.5.4.dylib
		fat_lib/libFosl_dynamic.dylib
		fat_lib/libft_malloc_x86_64_Darwin.so
		fat_lib/libgssapi_krb5.dylib
		fat_lib/libhunspell-1.2.dylib
		fat_lib/libicucore.dylib
		fat_lib/libinfo.dylib
		fat_lib/libk5crypto.dylib
		fat_lib/libkrb4.dylib
		fat_lib/libkrb524.dylib
		fat_lib/libkrb5.dylib
		fat_lib/libz.dylib
		fat_lib/libz.1.dylib
		fat_lib/libsqlite3.dylib
		fat_lib/libsqlite3.0.dylib
		fat_lib/libpmsample.dylib
		fat_lib/libform.dylib
		fat/fat_hard
		fat/MachO-OSX-ppc-and-i386-bash
		fat/MachO-iOS-armv7-armv7s-arm64-Helloworld
		32/MachO-OSX-ppc-openssl-1.0.1h
		lib_stat/lib_long_name.a
		lib_stat/libft_static.a
		fat/audiodevice
	)
	prefix="test/"
else
	nm_correct=(
		$1
	)
	otool_correct=(
		$1
	)
	prefix=""
fi

otool="otool -t" # ../UNIT_Factory/Nm-otool/ft_otool
for filename in "${otool_correct[@]}"; do
	./ft_otool $prefix$filename > /tmp/diff
	$otool $prefix$filename > /tmp/diff2
	res=$(diff /tmp/diff /tmp/diff2)
	if [[ ${res} = "" ]]; then
		printf "${GREEN}OTOOL: $filename: OK\n"
	else
		printf "${RED}OTOOL: $filename: KO\n${res}\n"
	fi
	printf "${NC}"
done

nm="nm" # ../UNIT_Factory/Nm-otool/ft_nm
for filename in "${nm_correct[@]}"; do
	./ft_nm $prefix$filename > /tmp/diff
	$nm $prefix$filename > /tmp/diff2
	res=$(diff /tmp/diff /tmp/diff2)
	if [[ ${res} = "" ]]; then
		printf "${GREEN}NM: $filename: OK\n"
	else
		printf "${RED}NM: $filename: KO\n${res}\n"
	fi
	printf "${NC}"
done

# -x tests
# nm="nm -x"
# for filename in "${nm_correct[@]}"; do
# 	./ft_nm -x $prefix$filename > /tmp/diff
# 	$nm $prefix$filename > /tmp/diff2
# 	res=$(diff /tmp/diff /tmp/diff2)
# 	if [[ ${res} = "" ]]; then
# 		printf "${GREEN}NM (-x): $filename: OK\n"
# 	else
# 		printf "${RED}NM (-x): $filename: KO\n${res}\n"
# 	fi
# 	printf "${NC}"
# done

# -p tests
# nm="nm -p"
# for filename in "${nm_correct[@]}"; do
# 	./ft_nm -p $prefix$filename > /tmp/diff
# 	$nm $prefix$filename > /tmp/diff2
# 	res=$(diff /tmp/diff /tmp/diff2)
# 	if [[ ${res} = "" ]]; then
# 		printf "${GREEN}NM (-p): $filename: OK\n"
# 	else
# 		printf "${RED}NM (-p): $filename: KO\n${res}\n"
# 	fi
# 	printf "${NC}"
# done

# -j tests
# nm="nm -j"
# for filename in "${nm_correct[@]}"; do
# 	./ft_nm -j $prefix$filename > /tmp/diff
# 	$nm $prefix$filename > /tmp/diff2
# 	res=$(diff /tmp/diff /tmp/diff2)
# 	if [[ ${res} = "" ]]; then
# 		printf "${GREEN}NM (-j): $filename: OK\n"
# 	else
# 		printf "${RED}NM (-j): $filename: KO\n${res}\n"
# 	fi
# 	printf "${NC}"
# done

# -r tests
# nm="nm -r"
# for filename in "${nm_correct[@]}"; do
# 	./ft_nm -r $prefix$filename > /tmp/diff
# 	$nm $prefix$filename > /tmp/diff2
# 	res=$(diff /tmp/diff /tmp/diff2)
# 	if [[ ${res} = "" ]]; then
# 		printf "${GREEN}NM: $filename: OK\n"
# 	else
# 		printf "${RED}NM: $filename: KO\n${res}\n"
# 	fi
# 	printf "${NC}"
# done

# -n tests
# nm="nm -n"
# for filename in "${nm_correct[@]}"; do
# 	./ft_nm -n $prefix$filename > /tmp/diff
# 	$nm $prefix$filename > /tmp/diff2
# 	res=$(diff /tmp/diff /tmp/diff2)
# 	if [[ ${res} = "" ]]; then
# 		printf "${GREEN}NM: $filename: OK\n"
# 	else
# 		printf "${RED}NM: $filename: KO\n${res}\n"
# 	fi
# 	printf "${NC}"
# done

# LC 0:
# corrupt/fat_not_fail_except_one

# nm fail:
# + /Users/osamoile/unit_test_nm_otool/custom_tests/test_wrong_lc_command_size                  ERROR: nm returned 1, ft_nm returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/bad_string_index                    ERROR: nm returned 0, ft_nm returned 1.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/fat_bad_string_index                ERROR: nm returned 0, ft_nm returned 1.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/fat_not_fail_except_one             ERROR: nm returned 1, ft_nm returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/indirect_bad_string                 ERROR: nm returned 0, ft_nm returned 1.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/mega_bad_string                     ERROR: nm returned 0, ft_nm returned 1.

# otool fail:
# + /Users/osamoile/unit_test_nm_otool/custom_tests/test_half_obj                               ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/test_wrong_lc_command_size                  ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/32/MachO-OSX-ppc-openssl-1.0.1h             ERROR: otool returned 0, ft_otool returned 1.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/64_corrupted_string_table           ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/bad_string_index                    ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/fat_bad_string_index                ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/fat_not_fail_except_one             ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/indirect_bad_string                 ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/mega_bad_string                     ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/corrupt/truncated_cputype                   ERROR: otool returned 1, ft_otool returned 0.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/fat/audiodevice                             ERROR: otool returned 0, ft_otool returned 1.
# + /Users/osamoile/unit_test_nm_otool/custom_tests/fat/MachO-OSX-ppc-and-i386-bash             ERROR: otool returned 0, ft_otool returned 1.

# Different error message:
# test_half_obj
# test_wrong_lc_command_size
# corrupt/audiodevice_corupted
# corrupt/bad_string_index
# corrupt/curl_truncated_load
# corrupt/truncated_10_load
# corrupt/empty
# corrupt/32_exe_hard_corupted
# corrupt/64_corrupted_string_table
# corrupt/64_exe_hard_corupted
# corrupt/lib_static.a
# corrupt/libmlx_corupted.a
# corrupt/mega_bad_string
# corrupt/truncated_load_2
# corrupt/fat_bad_string_index
# corrupt/fat_hard_corupted
# corrupt/indirect_bad_string
# corrupt/truncated_cputype
