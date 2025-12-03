#include "queue.h"
#include <stdexcept>

using namespace std;

CancelQueue::CancelQueue(const string& file) : filename(file) {
    try {
        load();
    }
    catch (const exception& e) {
        cout << "Предупреждение при загрузке очереди отмены: " << e.what() << endl;
    }
}
//функция записи строки в бинарный файл
void CancelQueue::writeString(ofstream& f, const string& s) {
    int len = (int)s.size();
    f.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        f.write(s.data(), len);
    }
    if (f.fail()) {
        throw runtime_error("Ошибка записи строки в файл");
    }
}
//функция чтения строки из бинарного файла
string CancelQueue::readString(ifstream& f) {
    int len = 0;
    f.read(reinterpret_cast<char*>(&len), sizeof(len));

    if (f.fail()) {
        throw runtime_error("Ошибка чтения длины строки из файла");
    }

    if (len < 0 || len > 1000000) {
        throw runtime_error("Некорректная длина строки в файле: " + to_string(len));
    }

    if (len == 0) {
        return "";
    }

    string s(len, '\0');
    f.read(&s[0], len);

    if (f.fail()) {
        throw runtime_error("Ошибка чтения данных строки из файла");
    }

    return s;
}
//функция сохранения в бинарный файл
void CancelQueue::save() {
    ofstream f(filename, ios::binary | ios::trunc);
    if (!f.is_open()) {
        throw runtime_error("Не удалось открыть файл для записи: " + filename);
    }
    queue<CancelRecord> tempQueue = queueData;
    int count = tempQueue.size();
    f.write(reinterpret_cast<char*>(&count), sizeof(count));

    while (!tempQueue.empty()) {
        const auto& record = tempQueue.front();
        writeString(f, record.patientFIO);
        writeString(f, record.visitRecord);
        tempQueue.pop();
    }

    if (f.fail()) {
        throw runtime_error("Ошибка записи данных в файл");
    }

    f.close();
}
//функция выгрузки из бинарного файла в память
void CancelQueue::load() {
    try {
        ifstream f(filename, ios::binary);
        if (!f.is_open()) {
            return;
        }

        int count;
        f.read(reinterpret_cast<char*>(&count), sizeof(count));

        if (f.fail()) {
            throw runtime_error("Ошибка чтения количества записей из файла");
        }

        for (int i = 0; i < count; i++) {
            string patientFIO = readString(f);
            string visitRecord = readString(f);
            queueData.push({ patientFIO, visitRecord });
        }

        f.close();
    }
    catch (const exception& e) {
        throw runtime_error("Ошибка загрузки очереди отмены");
    }
}
//добавляет запись в очередь отмены
void CancelQueue::addToQueue(const string& patientFIO, const string& record) {
    try {
        if (patientFIO.empty() || record.empty()) {
            throw invalid_argument("ФИО пациента и запись не могут быть пустыми");
        }

        queueData.push({ patientFIO, record });
        save();
        cout << "Запись добавлена в очередь отмены для пациента: " << patientFIO << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка добавления в очередь отмены: " << e.what() << endl;
        throw;
    }
}
//возвращает копию очереди отмены
queue<CancelRecord> CancelQueue::getQueue() {
    return queueData; 
}
//очищает очередь отмены
void CancelQueue::clearQueue() {
    while (!queueData.empty()) {
        queueData.pop();
    }
    save();
}
//возвращает пуста ли очередь
bool CancelQueue::isEmpty() const {
    return queueData.empty();
}
//возвращает размер очереди
int CancelQueue::size() const {
    return (int)queueData.size();
}