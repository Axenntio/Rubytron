#include <Shared/helper.hh>
#include <fstream>
#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_strm_mem.h>
#include <mz_zip_rw.h>

#ifdef _WIN32
    #include <windows.h>

    std::string pathToUtf8(const std::filesystem::path& path) {
        const std::wstring& wide = path.native();
        int len = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (len == 0) return "";

        std::string utf8(len - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, utf8.data(), len, nullptr, nullptr);
        return utf8;
    }
#else
    std::string pathToUtf8(const std::filesystem::path& path) {
        return path.string();
    }
#endif

bool addProgramToRuntime(unsigned char* zipContent, unsigned int zipLength, const std::string& filePath, const std::string& zipPath, const std::string& outputPath)
{
    std::vector<uint8_t> modified_zip;

    void* mem_stream_reader = mz_stream_mem_create();
    mz_stream_mem_set_buffer(mem_stream_reader, (void*)zipContent, zipLength);
    mz_stream_mem_open(mem_stream_reader, NULL, MZ_OPEN_MODE_READ);

	void* zip_reader = mz_zip_reader_create();
    if (mz_zip_reader_open(zip_reader, mem_stream_reader) != MZ_OK) {
        throw std::runtime_error("Failed to open zip from memory.");
		mz_zip_reader_delete(&zip_reader);
		mz_stream_mem_delete(&mem_stream_reader);
        return false;
    }

	void* mem_stream_writer = mz_stream_mem_create();
    mz_stream_mem_set_grow_size(mem_stream_writer, 1024 * 1024);
    mz_stream_mem_open(mem_stream_writer, NULL, MZ_OPEN_MODE_CREATE);

	void* zip_writer = mz_zip_writer_create();
    mz_zip_writer_set_compress_method(zip_writer, MZ_COMPRESS_METHOD_DEFLATE);
    mz_zip_writer_set_compress_level(zip_writer, 6);

	if (mz_zip_writer_open(zip_writer, mem_stream_writer, 0) != MZ_OK) {
        throw std::runtime_error("Failed to open zip writer to memory.");
        mz_zip_writer_delete(&zip_writer);
		mz_stream_mem_delete(&mem_stream_writer);
		mz_zip_reader_delete(&zip_reader);
		mz_stream_mem_delete(&mem_stream_reader);
        return false;
    }

	if (mz_zip_reader_goto_first_entry(zip_reader) == MZ_OK) {
        do {
            if (mz_zip_reader_entry_open(zip_reader) == MZ_OK) {
                if (mz_zip_writer_copy_from_reader(zip_writer, zip_reader) != MZ_OK) {
                    throw std::runtime_error("Failed to copy entry.");
                    mz_zip_reader_entry_close(zip_reader);
                    continue;
                }
                mz_zip_reader_entry_close(zip_reader);
            }
        } while (mz_zip_reader_goto_next_entry(zip_reader) == MZ_OK);
    }

	if (mz_zip_writer_add_file(zip_writer, pathToUtf8(std::filesystem::current_path() / filePath).c_str(), zipPath.c_str()) != MZ_OK) {
        throw std::runtime_error("Failed to add new file to ZIP.");

		mz_zip_writer_close(zip_writer);
		mz_zip_writer_delete(&zip_writer);
		mz_stream_mem_delete(&mem_stream_writer);
		mz_zip_reader_close(zip_reader);
		mz_zip_reader_delete(&zip_reader);
		mz_stream_mem_delete(&mem_stream_reader);
        return false;
    }

	mz_zip_writer_close(zip_writer);
    mz_zip_writer_delete(&zip_writer);

	mz_zip_reader_close(zip_reader);
    mz_zip_reader_delete(&zip_reader);
    mz_stream_mem_delete(&mem_stream_reader);

	const void* zipBuf = nullptr;
    int32_t zipBufLen = 0;
    mz_stream_mem_get_buffer(mem_stream_writer, &zipBuf);
    mz_stream_mem_get_buffer_length(mem_stream_writer, &zipBufLen);

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open output file.");
        mz_stream_mem_delete(&mem_stream_writer);
        return false;
    }
    out.write(reinterpret_cast<const char*>(zipBuf), zipBufLen);
    out.close();

    mz_stream_mem_delete(&mem_stream_writer);

	return true;
}