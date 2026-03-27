if(NOT WIN32)
	return()
endif()

if(NOT DEFINED exe OR NOT DEFINED dest)
	message(FATAL_ERROR "CopyRuntimeDeps.cmake requires -Dexe and -Ddest")
endif()

if(NOT EXISTS "${exe}")
	message(FATAL_ERROR "Executable not found: ${exe}")
endif()

if(NOT DEFINED toolchain_bin OR NOT EXISTS "${toolchain_bin}")
	message(FATAL_ERROR "CopyRuntimeDeps.cmake requires a valid -Dtoolchain_bin")
endif()

set(objdump_path "${toolchain_bin}/objdump.exe")
if(NOT EXISTS "${objdump_path}")
	message(FATAL_ERROR "objdump.exe not found in toolchain bin: ${objdump_path}")
endif()

file(MAKE_DIRECTORY "${dest}")

set(queue "${exe}")
set(processed_files)

while(queue)
	list(POP_FRONT queue current_file)

	list(FIND processed_files "${current_file}" already_processed)
	if(NOT already_processed EQUAL -1)
		continue()
	endif()
	list(APPEND processed_files "${current_file}")

	execute_process(
		COMMAND "${objdump_path}" -p "${current_file}"
		RESULT_VARIABLE dump_result
		OUTPUT_VARIABLE dump_output
		ERROR_QUIET
	)

	if(NOT dump_result EQUAL 0)
		continue()
	endif()

	string(REGEX MATCHALL "DLL Name: [^\r\n]+" dll_matches "${dump_output}")
	foreach(match IN LISTS dll_matches)
		string(REGEX REPLACE "DLL Name: " "" dll_name "${match}")
		set(candidate_path "${toolchain_bin}/${dll_name}")

		if(EXISTS "${candidate_path}")
			file(COPY "${candidate_path}" DESTINATION "${dest}")
			list(APPEND queue "${candidate_path}")
		endif()
	endforeach()
endwhile()

