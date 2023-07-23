# © 2023 Friendly Shade, Inc.
# © 2023 Sebastian Zapata
#
# This file is part of FileHauler.
# FileHauler is licensed under the GNU General Public License (GPL), Version 3. If a copy of the GPL
# was not distributed with this file, you can obtain one at https://www.gnu.org/licenses/gpl-3.0.

helper_add_executable(file_hauler
	OUTPUT_NAME FileHauler
	FOLDER "software"
	SOURCES
		"src/main.cpp"
		"src/operators.h"
		"src/Result.cpp"
		"src/Result.h"
		"src/Hauler.cpp"
		"src/Hauler.h"
		"src/Timer.cpp"
		"src/Timer.h"
		"src/util.h"
	LINKS ${LINKS}
)