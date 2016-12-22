#!/usr/bin/env sh

cat_license() {
    printf "\n=============================\n"
    printf "$1 license\n"
    printf "=============================\n"
    cat "$2"
}

main_dir=$1
printf ""
cat_license gta3sc "$main_dir/LICENSE"
cat_license cppformat "$main_dir/deps/cppformat/cppformat/LICENSE.rst"
cat_license expected "$main_dir/deps/expected/LICENSE"
cat_license SmallVector "$main_dir/deps/SmallVector/LICENSE.TXT"
cat_license string_view "$main_dir/deps/string_view/LICENSE.TXT"
printf "\n=============================\n\n" 
printf "Additionally any, optional, variant and rapidxml are licensed under the Boost Software License (http://www.boost.org/LICENSE_1_0.txt)\n" 
printf "\n=============================\n\n" 
