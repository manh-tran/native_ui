#
# Copyright (C) 2017 Manh Tran
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
mkdir -p ios_build
cmake -H. -DOUT=ios_build -Bios_build -GXcode -DCMAKE_BUILD_TYPE=Release -DUSER_BUILD_IOS=true -DAPP_DEBUG=true -DBUILD_NATIVE_UI_ONLY=true
cd ios_build
xcodebuild -project native_ui.xcodeproj -target ALL_BUILD -configuration Release OTHER_CFLAGS="-fembed-bitcode"
xcodebuild -project native_ui.xcodeproj -target ALL_BUILD -configuration Debug
cd ..
