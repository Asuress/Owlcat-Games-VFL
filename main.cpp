//Тестовое задание :
//
//Реализовать VFS(virtual file system).
//
//На большинстве игровых консолей сильно ограничено количество открытых файлов.Поэтому приходиться все сотни тысяч файлов данных игры паковать в несколько физических файлов на диске.
//
//Задача реализовать интерфейс IVFS - реализация должна все данные хранить в нескольких файлах на диске.
//
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <mutex>

namespace TestTask
{
	struct File
	{
	public:
		const char* fileName;
		std::fstream* fstream;
		std::ios_base::openmode mode;
		bool operator== (File& file) {
			return file.fileName == this->fileName ? true : false;
		}
	}; // Вы имеете право как угодно задать содержимое этой структуры

	struct IVFS
	{
	private:
		// не уверен, могу ли я добавлять свои поля и функцию, но по-идее для внешнего пользователя интерфейс не изменится
		std::vector<File*> openedFiles;
		std::mutex mtx;

		File* IsFileOpened(File file) {
			for (size_t i = 0; i < openedFiles.size(); i++)
			{
				if (openedFiles[i]->fileName == file.fileName)
					return openedFiles[i];
			}
			return nullptr;
		}

	public:
		File* Open(const char* name) {
			std::lock_guard<std::mutex> guard(mtx);

			std::fstream* fstreamFile = new std::fstream(name, std::ios_base::in | std::ios_base::app); // readonly mode

			if (!fstreamFile->is_open()) {
				fstreamFile->close();
				return nullptr;
			}

			File tmp;
			tmp.fileName = name;
			tmp.fstream = fstreamFile;
			tmp.mode = std::ios_base::in;
			
			auto openedFile = IsFileOpened(tmp);
			if (openedFile != nullptr ) 
			{
				if (openedFile->mode == std::ios_base::out)
				{
					fstreamFile->close();
					return nullptr;
				}
				if (openedFile->mode == std::ios_base::in)
				{
					return openedFile;
				}
				return nullptr;
			}

			File* file = new File();
			file->fileName = name;
			file->fstream = fstreamFile;
			file->mode = std::ios_base::in;

			openedFiles.push_back(file);

			return file;
		}; // Открыть файл в readonly режиме. Если нет такого файла или же он открыт во writeonly режиме - вернуть nullptr

		File* Create(const char* name) {
			std::lock_guard<std::mutex> guard(mtx);

			std::fstream* fstreamFile = new std::fstream(name, std::ios_base::out | std::ios_base::app);
			
			if (!fstreamFile->is_open()) {
				std::string str(name);
				std::filesystem::path path(name);
				
				bool isCreated = std::filesystem::create_directories(path);
				//std::filesystem::create_directory(path);
				fstreamFile->open(name, std::ios_base::out | std::ios_base::app);
			}

			File tmp;
			tmp.fileName = name;
			tmp.fstream = fstreamFile;
			tmp.mode = std::ios_base::out;
			//auto pos = std::find(openedFiles.begin(), openedFiles.end(), &tmp); // итератор, указывающий на позицию совпадения имени файла

			auto openedFile = IsFileOpened(tmp);
			if (openedFile != nullptr && openedFile->mode == std::ios_base::in) // файл открыт в readonly
			{
				return nullptr;
			}
			
			File* file = new File();
			file->fileName = name;
			file->fstream = fstreamFile;
			file->mode = std::ios_base::out;

			openedFiles.push_back(file);

			return file;
		}; // Открыть или создать файл в writeonly режиме. Если нужно, то создать все нужные поддиректории, упомянутые в пути. Вернуть nullptr, если этот файл уже открыт в readonly режиме.

		size_t Read(File* f, char* buff, size_t len) {
			if (f == nullptr) return 0;
			if (f->mode != std::ios_base::in)
			{
				return 0;
			}

			std::lock_guard<std::mutex> guard(mtx);
			auto initPos = f->fstream->tellg();
			f->fstream->read(buff, len);
			auto postPos = f->fstream->tellg();
			return postPos - initPos;
		}; // Прочитать данные из файла. Возвращаемое значение - сколько реально байт удалось прочитать

		size_t Write(File* f, char* buff, size_t len) {
			if (f == nullptr) return 0;
			if (f->mode != std::ios_base::out)
			{
				return 0;
			}

			std::lock_guard<std::mutex> guard(mtx);
			auto initPos = f->fstream->tellp();
			auto postPos = f->fstream->write(buff, len).tellp();
			return postPos - initPos;
		}; // Записать данные в файл. Возвращаемое значение - сколько реально байт удалось записать

		void Close(File* f) {
			if (f == nullptr) return;
			std::lock_guard<std::mutex> guard(mtx);
			if (f->fstream->is_open())
				f->fstream->close();

			if (IsFileOpened(*f) != nullptr)
			{
				auto i = openedFiles.begin();
				for (i; i != openedFiles.end(); i++)
				{
					if ((*i)->fileName == f->fileName)
					{
						delete *i;
						break;
					}
				}
				openedFiles.erase(i);
			}
		}; // Закрыть файл	
	};

}
//
//На что нужно обратить внимание :
//1. Возможна ситуация, когда есть одновременно открытые readonly и writeonly файлы(разные).
//2. Не забудьте про многопоточность.Работа с вашей системой возможна из нескольких потоков одновременно.

int main() {
	// test create_directory
	// протестить create()
	/*TestTask::IVFS ivfs;
	char* buff = new char[6]{'\0'};
	std::cout << buff << std::endl;

	const char* name = "a/b/test/a.txt";

	auto f = ivfs.Create(name);
	std::cout << ivfs.Write(f, (char*)"test", 5) << std::endl;
	ivfs.Close(f);

	f = ivfs.Open(name);
	std::cout << buff << std::endl;
	std::cout << ivfs.Read(f, buff, 5) << std::endl;
	std::cout << buff << std::endl;
	ivfs.Close(f);*/

	std::thread th1;



	th1.join();

	return 0;
}