#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <map>
#include <conio.h>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

#define BACKCOLOR 7
#define FRONTCOLOR 0
#define SELECTCOLOR 4
#define TOPMENU 5
#define LEFTMENU 20

void UA()
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	setlocale(LC_ALL, "uk_UA");
}
string GetDesktopPath()
{
	char* desktopPath = nullptr;
	size_t desktopPathSize = 0;
	if (_dupenv_s(&desktopPath, &desktopPathSize, "USERPROFILE") == 0)
	{
		string result(desktopPath);
		free(desktopPath);
		return result;
	}
	return "";  
}
void SetColor(unsigned short backColor, unsigned short frontColor)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD color = ((backColor & 0x0f) << 4) + (frontColor & 0x0f);
	SetConsoleTextAttribute(h, color);
}
void SetCursor(unsigned short row, unsigned short column)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c{ column,row };
	SetConsoleCursorPosition(h, c);
}
void PaintMenu(const char** textMenu, size_t sizeMenu, size_t select)
{
	SetColor(BACKCOLOR, FRONTCOLOR);
	SetCursor(TOPMENU, LEFTMENU);
	cout << "__________________________________________________________________________________";
	for (size_t i = 0; i < sizeMenu; i++)
	{
		SetCursor(TOPMENU + i + 1, LEFTMENU + 25);
		if (i == select)
			SetColor(BACKCOLOR, SELECTCOLOR);
		else
			SetColor(BACKCOLOR, FRONTCOLOR);
		cout << (i == select ? "=>" : " ") << textMenu[i];
		cout << "           ";
	}
	cout << "\n                    __________________________________________________________________________________";

	SetCursor(20, 0);
}
size_t Menu(const char** textMenu, size_t sizeMenu, size_t select)
{
	unsigned char c = 72;
	while (true)
	{
		if (c == 72 || c == 80)PaintMenu(textMenu, sizeMenu, select);
		c = _getch();
		switch (c)
		{
		case 13: return select;
		case 72:
			if (select == 0) select = sizeMenu;
			select--;
			break;//up
		case 80:
			select++;
			if (select == sizeMenu)
				select = 0;
			break;//down

		default:
			break;
		}
	}

	return select;
}
string sha512(const string& input) {
	unsigned char digest[SHA512_DIGEST_LENGTH];
	SHA512_CTX shaContext;
	SHA512_Init(&shaContext);
	SHA512_Update(&shaContext, input.c_str(), input.length());
	SHA512_Final(digest, &shaContext);

	stringstream ss;
	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
		ss << hex << setw(2) << setfill('0') << static_cast<int>(digest[i]);
	}

	return ss.str();
}

class User {
public:
	User() :User("", "", "", "", "", "") {}
	User(string login, string password, string name, string surname, string address, string phone, bool isAdmin = false)
		: login(login), password(password), name(name), surname(surname), address(address), phone(phone), isAdmin(isAdmin) {
	}
	string getName() const { return name; }
	bool getIsAdmin() const { return isAdmin; }
	void setIsAdmin(bool admin) { isAdmin = admin; }
	string getLogin() const { return login; }
	void setLogin(string l) { login = l; }
	string getPassword() const { return password; }
	void setPassword(string pass) { password = pass; }
	string GetName() const { return name; }
	void setName(const string& newName) { name = newName; }
	string GetSurname() const { return surname; }
	void setSurname(const string& newSurname) { surname = newSurname; }
	string GetAddress() const { return address; }
	void setAddress(const string& newAddress) { address = newAddress; }
	string GetPhone() const { return phone; }
	void setPhone(const string& newPhone) { phone = newPhone; }
	static bool IsLoginUnique(const string& login, const vector<User>& users) {
		return find_if(users.begin(), users.end(), [&login](const User& user) { return user.getLogin() == login; }) == users.end();
	}
	static User* Login(const string& login, const string& password, vector<User>& users) {
		auto userIterator = find_if(users.begin(), users.end(), [&login, &password](const User& user) {
			return user.getLogin() == login && user.getPassword() == password;
			});

		if (userIterator != users.end()) {
			return &(*userIterator);
		}

		return nullptr;
	}
	void ChangePassword(const string& newPassword) {

		this->password = newPassword;
	}
protected:
	bool isAdmin;
	string login;
	string password;
	string name;
	string surname;
	string address;
	string phone;
};
class Answer {
public:
	Answer() :Answer("", false) {}
	Answer(const string& text, bool isCorrect) : text(text), isCorrect(isCorrect) {}
	const string& getText() const {
		return text;
	}
	void setText(string text) { this->text = text; }
	bool isCorrectAnswer() const {
		return isCorrect;
	}
	void setCorrectness(bool correct) {
		isCorrect = correct;
	}
	friend ostream& operator<<(ostream& os, const Answer& answer) {
		os << "Текст питання: " << answer.text << ", Чи правильна: " << (answer.isCorrect ? "Так" : "Ні");
		return os;
	}
private:
	string text;
	bool isCorrect;
};
class Question {
public:
	Question() :Question("") {}
	Question(const string& questionText) : questionText(questionText) {}
	void addAnswer(const string& answerText, bool isCorrect) {
		answers.push_back(Answer(answerText, isCorrect));
	}
	const string& getQuestionText() const {
		return questionText;
	}
	const vector<Answer>& getAnswers() const {
		return answers;
	}
	void addQuestion(const string& questionText) {
		this->questionText = questionText;
		answers.clear();

	}
	string getName() const { return questionText; }
	void addAnswer(const Answer& answer) {
		answers.push_back(answer);
	}
	void setAnswerCorrectness(size_t answerIndex, bool isCorrect) {
		if (answerIndex < answers.size()) {
			answers[answerIndex].setCorrectness(isCorrect);
		}
	}
private:
	string questionText;
	vector<Answer> answers;
};
class Test {
public:
	Test() :testname("") {}
	Test(string testname) :testname(testname) {}
	string getTestName() const { return testname; }
	void setQuestions(const vector<Question>& newQuestions) {
		questions = newQuestions;
	}
	const vector<Question> getQuestions() const {
		return questions;
	}
	friend ostream& operator<<(ostream& os, const Test& test) {
		os << "Test: " << test.getTestName() << endl;

		return os;
	}
	void addQuestion(const Question& question) {
		questions.push_back(question);
	}
private:
	string testname;
	vector<Question> questions;
};
class Category {
public:
	Category() :Category("") {}
	Category(const string& categoryName) : categoryName(categoryName) {}
	const string& getCategoryName() const {
		return categoryName;
	}
	const vector<Test>& getTests() const {
		return tests;
	}
	string getCategoryName() { return categoryName; }
	void addTest(const Test& newTest) {
		tests.push_back(newTest);
	}
	friend ostream& operator<<(ostream& os, const Category& category) {
		os << "Category: " << category.getCategoryName() << endl;

		return os;
	}
	vector<Test>& getTests() {
		return tests;
	}
private:
	string categoryName;
	vector<Test> tests;
};
class Guest :public User
{
public:
	Guest(string login, string password, string name, string surname, string address, string phone)
		: User(login, password, name, surname, address, phone) {}
	Guest() :Guest("", "", "", "", "", "") {}
	struct TestResult {
		string testName;
		int totalQuestions;
		int correctAnswers;
		double percentage;
		int grade;
	};
	void takeTestInCategory(const vector<Category>& categories, vector<User> user, const string& userLogin) {
		// Виводимо доступні категорії
		SetCursor(20, 0);
		cout << "Доступні категорії:" << endl;
		for (size_t i = 0; i < categories.size(); ++i) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}

		// Запитуємо користувача про вибір категорії
		int categoryChoice;
		cout << "Виберіть категорію (1-" << categories.size() << "): ";
		cin >> categoryChoice;

		if (categoryChoice >= 1 && categoryChoice <= static_cast<int>(categories.size())) {
			const Category& selectedCategory = categories[categoryChoice - 1];
			// Виводимо тести в обраній категорії
			const vector<Test>& testsInCategory = selectedCategory.getTests();
			cout << "Тести у категорії '" << selectedCategory.getCategoryName() << "':" << endl;
			for (size_t i = 0; i < testsInCategory.size(); ++i) {
				cout << i + 1 << ". " << testsInCategory[i].getTestName() << endl;
			}

			// Запитуємо користувача про вибір тесту
			int testChoice;
			cout << "Виберіть тест (1-" << testsInCategory.size() << "): ";
			cin >> testChoice;

			if (testChoice >= 1 && testChoice <= static_cast<int>(testsInCategory.size())) {
				const Test& selectedTest = testsInCategory[testChoice - 1];
				takeTestAndSaveResults(selectedTest, userLogin);
			}
		}
		else {
			cout << "Неправильний вибір категорії." << endl;
		}
	}
	void takeTestAndSaveResults(const Test& test, const string& userLogin) {
		int totalQuestions = test.getQuestions().size();
		int correctAnswers = 0;

		// Створюємо змінну для зберігання відповідей користувача на кожне питання
		vector<bool> userAnswers(totalQuestions, false);

		// Проходимося по кожному питанню у тесті
		for (int i = 0; i < totalQuestions; ++i) {
			const vector<Question>& questions = test.getQuestions();
			if (i >= 0 && i < questions.size()) {
				const Question& question = questions[i];

				// Виводимо питання
				cout << "Питання " << (i + 1) << ": " << question.getQuestionText() << endl;

				// Виводимо варіанти відповідей
				const vector<Answer>& answers = question.getAnswers();
				for (size_t j = 0; j < answers.size(); ++j) {
					cout << j + 1 << ". " << answers[j].getText() << endl;
				}

				// Запитуємо користувача про вибір відповіді
				int userChoice;
				cout << "Виберіть варіант відповіді (1-" << answers.size() << "): ";
				cin >> userChoice;

				// Перевіряємо, чи вибрана відповідь правильна
				if (userChoice >= 1 && userChoice <= static_cast<int>(answers.size())) {
					if (answers[userChoice - 1].isCorrectAnswer()) {
						userAnswers[i] = true;
						correctAnswers++;
					}
				}
			}
		}
		// Виводимо результат тестування
		cout << "Результат тестування:" << endl;
		cout << "Кількість питань: " << totalQuestions << endl;
		cout << "Кількість правильних відповідей: " << correctAnswers << endl;
		double percentage = static_cast<double>(correctAnswers) / totalQuestions * 100.0;
		cout << "Відсоток правильних відповідей: " << percentage << "%" << endl;
		// Обчислюємо оцінку на 12-бальній системі
		int grade = static_cast<int>(percentage / 100.0 * 12);
		cout << "Оцінка: " << grade << "/12" << endl;
		TestResult result;
		result.testName = test.getTestName();
		result.totalQuestions = totalQuestions;
		result.correctAnswers = correctAnswers;
		result.percentage = percentage;
		result.grade = grade;
		testResults[userLogin] = result;
	}
	void loadResultsFromFile(const string& filename, const string& userLogin) {
		string desktopPath = GetDesktopPath();

		ifstream inputFile(desktopPath + "\\Desktop\\Testprogram\\" + filename);

		if (!inputFile.is_open()) {
			cout << "Не вдалося відкрити файл для заванаження результатів." << endl;
			return;
		}

		string line;
		TestResult result; // Створюємо об'єкт для збереження результату

		bool foundUser = false; // Змінна для відстеження, чи знайдено результати для поточного користувача

		while (getline(inputFile, line)) {
			if (line == "Логін: " + userLogin) {
				// Знайдено результат для поточного користувача, продовжуємо обробку
				foundUser = true;
			}
			else if (foundUser && line.find("Тест: ") == 0) {
				result.testName = line.substr(6); // Видаляємо префікс "Тест: "
			}
			else if (foundUser && line.find("Кількість питань: ") == 0) {
				result.totalQuestions = stoi(line.substr(18));
			}
			else if (foundUser && line.find("Кількість правильних відповідей: ") == 0) {
				result.correctAnswers = stoi(line.substr(33));
			}
			else if (foundUser && line.find("Відсоток правильних відповідей: ") == 0) {
				result.percentage = stod(line.substr(32));
			}
			else if (foundUser && line.find("Оцінка: ") == 0) {
				result.grade = stoi(line.substr(8));
			}
			else if (foundUser && line.find("-----------------------------------------") == 0) {
				// Завершили обробку одного результату, додаємо його до мапи
				testResults[userLogin] = result;
				foundUser = false; // Скидаємо прапорець знайденого користувача
			}
		}

		inputFile.close();
	}

	void saveResultsToFile(const string& filename, const string& userLogin) {
		string desktopPath = GetDesktopPath();
		ofstream outputFile(desktopPath + "\\Desktop\\testprogram\\" + filename, ios::app);
		if (outputFile.is_open())
		{
			for (const auto& pair : testResults)
			{
				const string& login = pair.first;
				const TestResult& result = pair.second;
				outputFile << "Логін: " + login << endl;
				outputFile << "Тест: " << result.testName << endl;
				outputFile << "Кількість питань: " << result.totalQuestions << endl;
				outputFile << "Кількість правильних відповідей: " << result.correctAnswers << endl;
				outputFile << "Відсоток правильних відповідей: " << result.percentage << "%" << endl;
				outputFile << "Оцінка: " << result.grade << "/12" << endl;
				outputFile << "-----------------------------------------" << endl;
			}
			outputFile.close();
		}
		else {
			cout << "Не вдалося відкрити файл для запису результатів." << endl;
		}
	}
	void addResult(const TestResult& result) {
		testResults[result.testName] = result;
	}
	void viewPreviousTestResults(const string& userLogin) {
		if (testResults.empty()) {
			cout << "Немає збережених результатів тестів." << endl;
			return;
		}

		cout << "Попередні результати тестів для користувача з логіном '" << userLogin << "':" << endl;
		for (const auto& pair : testResults) {
			if (pair.first == userLogin) {
				const TestResult& result = pair.second;
				cout << "Тест: " << result.testName << endl;
				cout << "Кількість питань: " << result.totalQuestions << endl;
				cout << "Кількість правильних відповідей: " << result.correctAnswers << endl;
				cout << "Відсоток правильних відповідей: " << result.percentage << "%" << endl;
				cout << "Оцінка: " << result.grade << "/12" << endl;
				cout << "-----------------------------------------" << endl;
			}
		}
	}
protected:
	map<string, TestResult> testResults;
};
class Admin : public User {
public:
	Admin(string login, string password, string name, string surname, string address, string phone, bool isAdmin = true)
		: User(login, password, name, surname, address, phone, isAdmin) {}
	void viewQuestionsAndAnswersInTest(const vector<Category>& categories) const {
		SetCursor(20, 0);
		cout << "Список доступних категорій:" << endl;
		for (size_t i = 0; i < categories.size(); i++) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}
		int categoryChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер категорії, в якій знаходиться тест: ";
			cin >> categoryChoice;

			// Перевірка чи ввід є числом
			if (cin.good()) {
				categoryChoice--;  // Ви зменшуєте на один для збігу з індексами вектора
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть цифру." << endl;
				cin.clear();      // Скидання прапорців помилки
				cin.ignore(100, '\n'); // Очищення буфера введення
			}
		}

		if (categoryChoice >= 0 && categoryChoice < static_cast<int>(categories.size())) {
			const Category& selectedCategory = categories[categoryChoice];

			cout << "Список тестів у категорії \"" << selectedCategory.getCategoryName() << "\":" << endl;
			for (size_t i = 0; i < selectedCategory.getTests().size(); i++) {
				cout << i + 1 << ". " << selectedCategory.getTests()[i].getTestName() << endl;
			}

			int testChoice;
			cout << "Виберіть номер тесту, для якого хочете переглянути питання та відповіді: ";
			cin >> testChoice;
			testChoice--;

			if (testChoice >= 0 && testChoice < static_cast<int>(selectedCategory.getTests().size())) {
				const Test& selectedTest = selectedCategory.getTests()[testChoice];
				cout << "Питання та відповіді для тесту \"" << selectedTest.getTestName() << "\":" << endl;

				const vector<Question> questions = selectedTest.getQuestions();
				for (size_t i = 0; i < questions.size(); i++) {
					const Question& question = questions[i];
					cout << "Питання " << i + 1 << ": " << question.getQuestionText() << endl;

					const vector<Answer> answers = question.getAnswers();
					for (size_t j = 0; j < answers.size(); j++) {
						const Answer& answer = answers[j];
						cout << "Відповідь " << j + 1 << ": " << answer.getText() << " - " << (answer.isCorrectAnswer() ? "Правильна" : "Неправильна") << endl;
					}
					cout << "------------------------------------------------" << endl;
				}
			}
		}
	}
	void printAllTestsInCategory(vector<Category>& categories) const {
		cout << "Список доступних категорій:" << endl;
		for (size_t i = 0; i < categories.size(); i++) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}
		int userChoice;
		cout << "Виберіть  категорію, в якої хочете подивитися тести: ";
		cin >> userChoice;
		userChoice--;
		if (userChoice >= 0 && userChoice < static_cast<int>(categories.size())) {

			for (const auto& item : categories[userChoice].getTests())
				cout << item << "\n";
		}

	}
	void addCategory(vector<Category>& categories) {
		SetCursor(20, 0);

		cout << "Введіть назву категорії: ";
		string temp;
		cin >> temp;
		cin.ignore();
		categories.push_back(Category(temp));
		cout << "Категорія \"" << temp << "\" успішно створена." << endl;
	}
	void addTestToCategory(vector<Category>& categories) {
		SetCursor(20, 0);

		cout << "Список доступних категорій:" << endl;
		for (size_t i = 0; i < categories.size(); i++) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}
		int userChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер категорії, в якій знаходиться тест: ";
			cin >> userChoice;
			if (cin.good()) {
				userChoice--;
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть цифру." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}
		cout << "Введіть назву тесту: ";
		string temp;
		cin >> temp;
		cin.ignore();


		if (userChoice >= 0 && userChoice < static_cast<int>(categories.size())) {
			categories[userChoice].addTest(temp);
			cout << "Тест \"" << temp << "\" успішно доданий до категорії \"" << categories[userChoice].getCategoryName() << "\"." << endl;
		}
		else {
			cout << "Неправильний вибір категорії." << endl;
			cin.ignore();
		}
	}
	void addQuestionToTest(vector<Category>& categories) {
		SetCursor(20, 0);

		cout << "Список доступних категорій:" << endl;
		for (size_t i = 0; i < categories.size(); i++) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}
		int categoryChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер категорії, в якій знаходиться тест: ";
			cin >> categoryChoice;

			if (cin.good()) {
				categoryChoice--;  // Ви зменшуєте на один для збігу з індексами вектора
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть цифру." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		if (categoryChoice >= 0 && categoryChoice < static_cast<int>(categories.size())) {
			Category& selectedCategory = categories[categoryChoice];

			cout << "Список тестів у категорії \"" << selectedCategory.getCategoryName() << "\":" << endl;
			for (size_t i = 0; i < selectedCategory.getTests().size(); i++) {
				cout << i + 1 << ". " << selectedCategory.getTests()[i].getTestName() << endl;
			}

			int testChoice;
			cout << "Виберіть номер тесту, до якого хочете додати питання: ";
			cin.ignore();
			cin >> testChoice;
			testChoice--;

			if (testChoice >= 0 && testChoice < static_cast<int>(selectedCategory.getTests().size())) {
				Test& selectedTest = const_cast<Test&>(selectedCategory.getTests()[testChoice]);
				Question newQuestion;
				cout << "Введіть текст питання: ";
				cin.ignore();
				string questionText;
				getline(cin, questionText);
				newQuestion.addQuestion(questionText);
				for (int i = 0; i < 3; i++) {
					cout << "Введіть текст відповіді " << i + 1 << ": ";
					string answerText;
					getline(cin, answerText);
					cin.ignore();
					int a;
					cout << "Це правильна відповідь? (1 - Так, 2 - Ні): ";
					cin >> a;
					cin.ignore();
					bool isCorrect = (a == 1) ? true : false;

					newQuestion.addAnswer(answerText, isCorrect);
				}

				selectedTest.addQuestion(newQuestion);
				cout << "Питання успішно додане до тесту \"" << selectedTest.getTestName() << "\"." << endl;
			}
			else {
				cout << "Неправильний вибір тесту." << endl;
			}
		}
	}
	void removeCategory(vector<Category>& categories) {
		SetCursor(20, 0);

		cout << "Список доступних категорій:" << endl;
		for (size_t i = 0; i < categories.size(); i++) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}

		int categoryChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер категорії для видалення: ";
			cin >> categoryChoice;

			if (cin.good() && categoryChoice > 0 && categoryChoice <= static_cast<int>(categories.size())) {
				categoryChoice--;  // Зменшуємо на один для збігу з індексами вектора
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть правильний номер категорії." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		const string& categoryNameToDelete = categories[categoryChoice].getCategoryName();

		// Видаляємо категорію та всі тести та питання пов'язані з нею
		auto it = remove_if(categories.begin(), categories.end(), [categoryNameToDelete](const Category& category) {
			return category.getCategoryName() == categoryNameToDelete;
			});

		if (it != categories.end()) {
			categories.erase(it, categories.end());
			cout << "Категорію \"" << categoryNameToDelete << "\" видалено разом з усіма тестами та питаннями." << endl;
		}
		else {
			cout << "Категорію \"" << categoryNameToDelete << "\" не знайдено." << endl;
		}
	}
	void removeTestFromCategory(vector<Category>& categories) {
		SetCursor(20, 0);

		// Виводимо список доступних категорій
		cout << "Список доступних категорій:" << endl;
		for (size_t i = 0; i < categories.size(); i++) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}

		int categoryChoice;
		bool validCategoryInput = false;

		while (!validCategoryInput) {
			cout << "Виберіть номер категорії, з якої хочете видалити тест: ";
			cin >> categoryChoice;

			if (cin.good() && categoryChoice > 0 && categoryChoice <= static_cast<int>(categories.size())) {
				categoryChoice--;  // Зменшуємо на один для збігу з індексами вектора
				validCategoryInput = true;
			}
			else {
				cout << "Неправильний ввід для категорії. Будь ласка, введіть правильний номер категорії." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		// Виводимо список тестів вибраної категорії
		cout << "Список тестів у категорії \"" << categories[categoryChoice].getCategoryName() << "\":" << endl;
		for (size_t i = 0; i < categories[categoryChoice].getTests().size(); i++) {
			cout << i + 1 << ". " << categories[categoryChoice].getTests()[i].getTestName() << endl;
		}

		int testChoice;
		bool validTestInput = false;

		while (!validTestInput) {
			cout << "Виберіть номер тесту для видалення: ";
			cin >> testChoice;

			if (cin.good() && testChoice > 0 && testChoice <= static_cast<int>(categories[categoryChoice].getTests().size())) {
				testChoice--;  // Зменшуємо на один для збігу з індексами вектора
				validTestInput = true;
			}
			else {
				cout << "Неправильний ввід для тесту. Будь ласка, введіть правильний номер тесту." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		// Отримуємо ім'я тесту, який потрібно видалити
		const string& testNameToDelete = categories[categoryChoice].getTests()[testChoice].getTestName();

		// Отримуємо вектор тестів вибраної категорії
		vector<Test>& testsInCategory = categories[categoryChoice].getTests();

		// Знаходимо індекс тесту, який потрібно видалити
		int indexToDelete = -1;
		for (size_t i = 0; i < testsInCategory.size(); i++) {
			if (testsInCategory[i].getTestName() == testNameToDelete) {
				indexToDelete = i;
				break;
			}
		}

		if (indexToDelete != -1) {
			// Видаляємо тест із обраної категорії за індексом
			testsInCategory.erase(testsInCategory.begin() + indexToDelete);
			cout << "Тест \"" << testNameToDelete << "\" видалено з категорії \"" << categories[categoryChoice].getCategoryName() << "\"." << endl;
		}
		else {
			cout << "Тест \"" << testNameToDelete << "\" не знайдено в категорії \"" << categories[categoryChoice].getCategoryName() << "\"." << endl;
		}
	}
	void saveDataToFile(const vector<Category>& categories, const string& filename) {
		string desktopPath = GetDesktopPath();

		ofstream outFile(desktopPath + "\\Desktop\\Testprogram\\" + filename, ios::app);
		if (!outFile.is_open()) {
			cout << "Не вдалося відкрити файл для збереження даних." << endl;
			return;
		}

		for (const Category& category : categories) {
			outFile << "___________________________________\n";
			outFile << "Category: " << category.getCategoryName() << endl;

			for (const Test& test : category.getTests()) {
				outFile << "Test: " << test.getTestName() << endl;

				for (const Question& question : test.getQuestions()) {
					outFile << "Question: " << question.getQuestionText() << endl;

					for (const Answer& answer : question.getAnswers()) {
						outFile << "Answer: " << answer.getText() << " - " << (answer.isCorrectAnswer() ? "Correct" : "Incorrect") << endl;
					}
				}
				outFile << endl; // Додати порожній рядок між питаннями.
			}
			outFile << endl; // Додати порожній рядок між тестами.
		}
		outFile.close();
		cout << "Дані збережено у файл " << filename << " успішно." << endl;
	}
	void importDataFromFile(vector<Category>& categories, const string& filename) {
		string desktopPath = GetDesktopPath();

		ifstream inFile(desktopPath + "\\Desktop\\Testprogram\\" + filename);
		if (!inFile.is_open()) {
			cout << "Не вдалося відкрити файл для завантаження даних." << endl;
			return;
		}

		categories.clear();

		string line;
		Category currentCategory;
		Test currentTest;
		Question currentQuestion;

		while (getline(inFile, line)) {
			if (line.find("Category: ") == 0) {
				if (!currentCategory.getCategoryName().empty()) {
					if (!currentTest.getTestName().empty()) {
						currentCategory.addTest(currentTest);
					}
					categories.push_back(currentCategory);
				}
				currentCategory = Category(line.substr(10));
				currentTest = Test();
			}
			else if (line.find("Test: ") == 0) {
				if (!currentTest.getTestName().empty()) {
					if (!currentQuestion.getQuestionText().empty()) {
						currentTest.addQuestion(currentQuestion);
					}
					currentCategory.addTest(currentTest);

				}
				currentTest = Test(line.substr(6));
				currentQuestion = Question();
			}
			else if (line.find("Question: ") == 0) {
				if (!currentQuestion.getQuestionText().empty()) {
					currentTest.addQuestion(currentQuestion);
				}
				currentQuestion = Question(line.substr(9));
			}
			else if (line.find("Answer: ") == 0) {
				string answerText = line.substr(8, line.find(" - ") - 8);
				string isCorrect = line.substr(line.find(" - ") + 3);
				bool correct = (isCorrect == "Correct");
				currentQuestion.addAnswer(Answer(answerText, correct));

			}
			else if (line.find("___________________________________\n") == 0)
			{
				categories.push_back(currentCategory);
			}
		}
		if (!currentCategory.getCategoryName().empty()) {
			if (!currentTest.getTestName().empty()) {
				if (!currentQuestion.getQuestionText().empty()) {
					currentTest.addQuestion(currentQuestion);
				}
				currentCategory.addTest(currentTest);
			}
			categories.push_back(currentCategory);
		}

		// Додати останні дані з файлу до структури.

		inFile.close();
		cout << "Дані завантажено з файлу " << filename << " успішно." << endl;
	}
	void viewResultsByCategory(const vector<Category>& categories, const map<string, Guest::TestResult>& testResults) const {
		SetCursor(20, 0);
		cout << "Результати тестування за категоріями:" << endl;

		cout << "Список доступних категорій:" << endl;
		for (size_t i = 0; i < categories.size(); i++) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}
		int categoryChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер категорії, в якій знаходиться тест: ";
			cin >> categoryChoice;

			if (cin.good()) {
				categoryChoice--;  // Ви зменшуєте на один для збігу з індексами вектора
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть цифру." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		if (categoryChoice >= 0 && categoryChoice < static_cast<int>(categories.size())) {
			const Category& selectedCategory = categories[categoryChoice];  // Отримуємо вибрану категорію

			cout << "Категорія: " << selectedCategory.getCategoryName() << endl;
			for (const Test& test : selectedCategory.getTests()) {
				cout << "Тест: " << test.getTestName() << endl;
				int totalQuestions = 0;
				int totalCorrectAnswers = 0;

				for (const auto& pair : testResults) {
					const Guest::TestResult& result = pair.second;
					if (result.testName == test.getTestName()) {
						totalQuestions += result.totalQuestions;
						totalCorrectAnswers += result.correctAnswers;
					}
				}

				if (totalQuestions > 0) {
					double percentage = static_cast<double>(totalCorrectAnswers) / totalQuestions * 100.0;
					int grade = static_cast<int>(percentage / 100.0 * 12);
					cout << "Загальна кількість питань: " << totalQuestions << endl;
					cout << "Загальна кількість правильних відповідей: " << totalCorrectAnswers << endl;
					cout << "Загальний відсоток правильних відповідей: " << percentage << "%" << endl;
					cout << "Загальна оцінка: " << grade << "/12" << endl;
					cout << "-----------------------------------------" << endl;
				}
			}
		}
	}
	void viewResultsByTest(const vector<Category>& categories, const map<string, Guest::TestResult>& testResults) const {
		SetCursor(20, 0);
		cout << "Результати тестування за конкретним тестом:" << endl;

		cout << "Список доступних категорій:" << endl;
		for (size_t i = 0; i < categories.size(); i++) {
			cout << i + 1 << ". " << categories[i].getCategoryName() << endl;
		}
		int categoryChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер категорії, в якій знаходиться тест: ";
			cin >> categoryChoice;

			if (cin.good()) {
				categoryChoice--;
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть цифру." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		if (categoryChoice >= 0 && categoryChoice < static_cast<int>(categories.size())) {
			const Category& selectedCategory = categories[categoryChoice];

			cout << "Категорія: " << selectedCategory.getCategoryName() << endl;
			const vector<Test>& tests = selectedCategory.getTests();

			cout << "Список доступних тестів у цій категорії:" << endl;
			for (size_t i = 0; i < tests.size(); i++) {
				cout << i + 1 << ". " << tests[i].getTestName() << endl;
			}

			int testChoice;
			validInput = false;

			while (!validInput) {
				cout << "Виберіть номер тесту для перегляду результатів: ";
				cin >> testChoice;

				if (cin.good()) {
					testChoice--;
					validInput = true;
				}
				else {
					cout << "Неправильний ввід. Будь ласка, введіть цифру." << endl;
					cin.clear();
					cin.ignore(100, '\n');
				}
			}

			if (testChoice >= 0 && testChoice < static_cast<int>(tests.size())) {
				const Test& selectedTest = tests[testChoice];

				cout << "Тест: " << selectedTest.getTestName() << endl;
				int totalQuestions = 0;
				int totalCorrectAnswers = 0;

				for (const auto& pair : testResults) {
					const Guest::TestResult& result = pair.second;
					if (result.testName == selectedTest.getTestName()) {
						totalQuestions += result.totalQuestions;
						totalCorrectAnswers += result.correctAnswers;
					}
				}

				if (totalQuestions > 0) {
					double percentage = static_cast<double>(totalCorrectAnswers) / totalQuestions * 100.0;
					int grade = static_cast<int>(percentage / 100.0 * 12);
					cout << "Загальна кількість питань: " << totalQuestions << endl;
					cout << "Загальна кількість правильних відповідей: " << totalCorrectAnswers << endl;
					cout << "Загальний відсоток правильних відповідей: " << percentage << "%" << endl;
					cout << "Загальна оцінка: " << grade << "/12" << endl;
				}
			}
		}
	}
	void viewResultsByUsers(const vector<User>& users, const map<string, Guest::TestResult>& testResults) const {
		SetCursor(20, 0);
		cout << "Перегляд результатів тестування за конкретними користувачами:" << endl;

		cout << "Список доступних користувачів:" << endl;
		for (size_t i = 0; i < users.size(); i++) {
			cout << i + 1 << ". " << users[i].getLogin() << endl;
		}

		int userChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер користувача для перегляду результатів: ";
			cin >> userChoice;

			if (cin.good()) {
				userChoice--;  // Ви зменшуєте на один для збігу з індексами вектора
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть цифру." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		if (userChoice >= 0 && userChoice < static_cast<int>(users.size())) {
			const User& selectedUser = users[userChoice];

			cout << "Користувач: " << selectedUser.getLogin() << endl;

			for (const auto& pair : testResults) {
				const string& userLogin = selectedUser.getLogin();
				const Guest::TestResult& result = pair.second;

				if (userLogin == pair.first) {
					cout << "Тест: " << result.testName << endl;
					cout << "Кількість питань: " << result.totalQuestions << endl;
					cout << "Кількість правильних відповідей: " << result.correctAnswers << endl;
					double percentage = static_cast<double>(result.correctAnswers) / result.totalQuestions * 100.0;
					int grade = static_cast<int>(percentage / 100.0 * 12);
					cout << "Відсоток правильних відповідей: " << percentage << "%" << endl;
					cout << "Оцінка: " << grade << "/12" << endl;
					cout << "-----------------------------------------" << endl;
				}
			}
		}
	}
	void addUser(vector<User>& users) {
		// Перевірте, чи логін унікальний, і якщо так, створіть та додайте нового користувача в вектор користувачів.
		string login__, pass, name, surname, address, phone;

		SetCursor(5, 40);
		cout << "Введіть логін: ";
		cin >> login__;
		if (User::IsLoginUnique(login__, users))
		{
			SetCursor(6, 40);
			cout << "Введіть пароль: ";
			cin >> pass;
			SetCursor(7, 40);
			cout << "Введіть ім'я: ";
			cin >> name;
			SetCursor(8, 40);
			cout << "Введіть прізвище: ";
			cin >> surname;
			SetCursor(9, 40);
			cout << "Введіть адресу: ";
			cin >> address;
			SetCursor(10, 40);
			cout << "Введіть номер телефону: ";
			cin >> phone;
			system("cls");
			cin.ignore();
			users.push_back(User(login__, sha512(pass), name, surname, address, phone));
			SetCursor(20, 0);
			cout << "Користувач " << name << " успішно створений." << endl;
		}
		else {
			SetCursor(20, 0);
			cout << "Користувач з логіном \"" << login__ << "\" вже існує." << endl;
		}
	}
	void removeUser(vector<User>& users, const string& login) {
		// Виводимо список користувачів для вибору користувача, якого треба видалити
		SetCursor(20, 0);
		cout << "Список доступних користувачів:" << endl;
		for (size_t i = 0; i < users.size(); i++) {
			cout << i + 1 << ". " << users[i].getLogin() << endl;
		}

		int userChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер користувача, якого ви бажаєте видалити: ";
			cin >> userChoice;

			if (cin.good() && userChoice > 0 && userChoice <= static_cast<int>(users.size())) {
				userChoice--;  // Зменшуємо на один для збігу з індексами вектора
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть правильний номер користувача." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		const string& selectedUserLogin = users[userChoice].getLogin();

		auto it = remove_if(users.begin(), users.end(), [selectedUserLogin](const User& user) {
			return user.getLogin() == selectedUserLogin;
			});

		if (it != users.end()) {
			users.erase(it, users.end());
			cout << "Користувач з логіном " << selectedUserLogin << " видалений." << endl;
		}
		else {
			cout << "Користувач з логіном " << selectedUserLogin << " не знайдений." << endl;
		}
	}
	void modifyUser(vector<User>& users) {
		SetCursor(20, 0);
		cout << "Список доступних користувачів:" << endl;
		for (size_t i = 1; i < users.size(); i++) {
			cout << i + 1 << ". " << users[i].getLogin() << endl;
		}

		int userChoice;
		bool validInput = false;

		while (!validInput) {
			cout << "Виберіть номер користувача для редагування: ";
			cin >> userChoice;

			if (cin.good() && userChoice > 1 && userChoice <= static_cast<int>(users.size())) {
				validInput = true;
			}
			else {
				cout << "Неправильний ввід. Будь ласка, введіть правильний номер користувача." << endl;
				cin.clear();
				cin.ignore(100, '\n');
			}
		}

		// Отримуємо посилання на обраного користувача
		User& selectedUser = users[userChoice];

		// Запитуємо нові дані від користувача
		string  pass, name, surname, address, phone;

		cout << "Введіть новий пароль: ";
		cin >> pass;
		cout << "Введіть нове ім'я: ";
		cin >> name;
		cout << "Введіть нове прізвище: ";
		cin >> surname;
		cout << "Введіть нову адресу: ";
		cin >> address;
		cout << "Введіть новий номер телефону: ";
		cin >> phone;

		// Змінюємо дані користувача на нові
		selectedUser.setPassword(sha512(pass));
		selectedUser.setName(name);
		selectedUser.setSurname(surname);
		selectedUser.setAddress(address);
		selectedUser.setPhone(phone);
		cout << "Дані користувача оновлено успішно." << endl;


	}
	void ViewAllUsers(const vector<User>& users) {
		system("cls");
		SetCursor(20, 0);
		cout << "Список всіх користувачів:" << endl;
		for (const User& user : users) {
			cout << "Логін: " << user.getLogin() << endl;
			cout << "Ім'я: " << user.GetName() << endl;
			cout << "Прізвище: " << user.GetSurname() << endl;
			cout << "Адреса: " << user.GetAddress() << endl;
			cout << "Телефон: " << user.GetPhone() << endl;
			cout << "------------------------------------------------" << endl;
		}
	}
};
void LoadResultsFromFile(const string& filename, map<string, Guest::TestResult>& testResults) {
	string desktopPath = GetDesktopPath();

	ifstream inputFile(desktopPath + "\\Desktop\\Testprogram\\" + filename);

	if (!inputFile.is_open()) {
		cout << "Не вдалося відкрити файл для завантаження результатів." << endl;
		return;
	}

	string line;
	Guest::TestResult result; 
	string currentUser; 
	while (getline(inputFile, line)) {
		if (line.find("Логін: ") == 0) {
			
			currentUser = line.substr(7);
		}
		else if (line.find("Тест: ") == 0) {
			result.testName = line.substr(6); 
		}
		else if (line.find("Кількість питань: ") == 0) {
			result.totalQuestions = stoi(line.substr(18));
		}
		else if (line.find("Кількість правильних відповідей: ") == 0) {
			result.correctAnswers = stoi(line.substr(33));
		}
		else if (line.find("Відсоток правильних відповідей: ") == 0) {
			result.percentage = stod(line.substr(32));
		}
		else if (line.find("Оцінка: ") == 0) {
			result.grade = stoi(line.substr(8));
		}
		else if (line.find("-----------------------------------------") == 0) {
			testResults[currentUser] = result;
		}
	}
	cout << "Дані завантажено з файлу " << filename << " успішно." << endl;

	inputFile.close();
}

void SaveUsersToFile(const vector<User>& users, const string& filename) {
	string desktopPath = GetDesktopPath();// +"\\OneDrive\\Desktop\\testprogram";

	if (!desktopPath.empty())
	{
		fs::create_directories(desktopPath + "\\Desktop\\Testprogram\\");

		ofstream file(desktopPath + "\\Desktop\\Testprogram\\" + filename, ios::out);
		if (file.is_open())
		{
			for (const User& user : users)
			{
				file << "Login: " << user.getLogin() << endl;
				file << "Password: " << user.getPassword() << endl;
				file << "Name: " << user.getName() << endl;
				file << "Surname: " << user.GetSurname() << endl;
				file << "Address: " << user.GetAddress() << endl;
				file << "Phone: " << user.GetPhone() << endl;
				file << "Is Admin: " << (user.getIsAdmin() ? "Yes" : "No") << endl;
				file << endl;
			}
			file.close();
			cout << "Користувачі збережені в  " << desktopPath + "\\Desktop\\Testprogram\\" + filename << endl;
		}
		else
		{
			cerr << "Неможливо відкрити файл для запису" << endl;
		}
	}
	else {
		cerr << "Неможливо отримати шлях до робочого столу" << endl;
	}
}
void LoadUsersFromFile(vector<User>& users, const string& filename) {
	string desktopPath = GetDesktopPath();

	ifstream file(desktopPath + "\\Desktop\\Testprogram\\" + filename);

	if (file.is_open()) {
		string line;
		User currentUser;

		while (getline(file, line)) {
			if (line.empty()) {
				users.push_back(currentUser);
				currentUser = User();
			}
			else if (line.find("Login:") != string::npos) {
				currentUser.setLogin(line.substr(line.find(":") + 1));
			}
			else if (line.find("Password:") != string::npos) {
				currentUser.setPassword(line.substr(line.find(":") + 1));
			}
			else if (line.find("Name:") != string::npos) {
				currentUser.setName(line.substr(line.find(":") + 1));
			}
			else if (line.find("Surname:") != string::npos) {
				currentUser.setSurname(line.substr(line.find(":") + 1));
			}
			else if (line.find("Address:") != string::npos) {
				currentUser.setAddress(line.substr(line.find(":") + 1));
			}
			else if (line.find("Phone:") != string::npos) {
				currentUser.setPhone(line.substr(line.find(":") + 1));
			}
			else if (line.find("Is Admin: Yes") != string::npos) {
				currentUser.setIsAdmin(true);
			}
		}
		cout << "Дані завантажено з файлу " << filename << " успішно." << endl;

		file.close();
	}
	else {
		cerr << "Неможливо відкрити файл для читання." << endl;
	}
}
string trim(const string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	size_t last = str.find_last_not_of(" \t\n\r");
	if (first == string::npos) {
		return "";
	}
	return str.substr(first, (last - first + 1));
}
int main()
{
	UA();
	vector<User> users;
	vector<User> loadedUsers;
	string login_, pass, name, surname, address, phone, temp, temp2, temp3;
	Admin adminUser("admin", sha512("admin"), "Admin", "Vadym", "Kovel", "380987456355", true);
	Guest guest;
	vector<User>::iterator userIterator;
	vector<Category> cat;
	map<string, Guest::TestResult> testResults;
	bool loggedIn = false;
	SetColor(BACKCOLOR, FRONTCOLOR);
	system("cls");
	size_t select = 0;
	const char* textMenu[] = { "Завантажити базу даних",
							   "Увійти",
							   "Exit" };
	size_t sizeMenu = sizeof(textMenu) / sizeof(*textMenu);

	const char* textAdminMenu[] = { "Управління користувачами",
									"Перегляд статистики",
									"Управління тестуванням",
									"Змінити пароль Адміністратора",
									"Exit" };
	size_t sizeAdminMenu = sizeof(textAdminMenu) / sizeof(*textAdminMenu);

	const char* textAdminPidMenuUser[] = { "Додати користувача",
									"Видалити користувача",
									"Редагувати користувача",
									"Перегляд всіх користувачів",
									"Зберегти користувачів",
									"Exit" };
	size_t sizeAdminPidMenuUser = sizeof(textAdminPidMenuUser) / sizeof(*textAdminPidMenuUser);

	const char* textAdminPidMenuStatistic[] = { "Перегляд результатів тестування загалом за категоріями",
									"Перегляд результатів тестування за конкретними тестами",
									"Перегляд результатів тестування за конкретними користувачами",
									"Exit" };
	size_t sizeAdminPidMenuStatistic = sizeof(textAdminPidMenuStatistic) / sizeof(*textAdminPidMenuStatistic);

	const char* textAdminPidMenuTest[] = { "Додати категорію",
									"Додати тест до категорії",
									"Додати питання до тесту",
									"Видалити категорію",
									"Видалити тест",
									"Показати всі категорії",
									"Показати всі тести",
									"Показати питання і відповіді в конкретного теста",
									"Зберегти у файл",
									"Завантажити з файлу",
									"Exit" };
	size_t sizeAdminPidMenuTest = sizeof(textAdminPidMenuTest) / sizeof(*textAdminPidMenuTest);

	const char* textGuestMenu[] = { "Пройти тестування",
									"Перегляд результатів",
									"Змінити пароль",
									"Exit" };
	size_t sizeGuestMenu = sizeof(textGuestMenu) / sizeof(*textGuestMenu);
	while (1)
	{
		select = Menu(textMenu, sizeMenu, select);
		system("cls");
		switch (select)
		{
		case 0:
		{
			LoadUsersFromFile(loadedUsers, "users.txt");
			for (const User& loadedUser : loadedUsers) {
				string loadedLogin = loadedUser.getLogin();
				loadedLogin = trim(loadedLogin);
				const string& loadedPasswordHash = loadedUser.getPassword();
				if (loadedLogin == adminUser.getLogin())
				{
					adminUser.setPassword(trim(loadedPasswordHash));
					users.push_back(adminUser);
				}
				else
				{
					users.push_back(User(loadedLogin, trim(loadedPasswordHash), trim(loadedUser.GetName()), trim(loadedUser.GetSurname()), trim(loadedUser.GetAddress()), trim(loadedUser.GetPhone())));
				}
			}
			adminUser.importDataFromFile(cat, "category.txt");
			LoadResultsFromFile("results.txt", testResults);
			break;
		}
		case 1:
		{
			if (users.empty())
				users.push_back(adminUser);
			SetColor(BACKCOLOR, FRONTCOLOR);
			SetCursor(5, 40);
			bool loggedIn = false;
			User* loggedInUser = nullptr;
			while (!loggedIn)
			{
				SetCursor(6, 40);
				cout << "Введіть логін: ";
				cin >> login_;

				if (User::IsLoginUnique(login_, users))
				{
					SetCursor(6, 0);
					cout << "Продовжіть реєстрацію користувача з логіном " << login_ << ", після реєстрації увійдіть у систему використовуючи ваш логін та пароль\n";
					SetCursor(8, 40);
					cout << "Введіть пароль: ";
					cin >> pass;
					SetCursor(9, 40);
					cout << "Введіть ім'я: ";
					cin >> name;
					SetCursor(10, 40);
					cout << "Введіть прізвище: ";
					cin >> surname;
					SetCursor(11, 40);
					cout << "Введіть адресу: ";
					cin >> address;
					SetCursor(12, 40);
					cout << "Введіть номер телефону: ";
					cin >> phone;
					users.push_back(User(login_, sha512(pass), name, surname, address, phone));
					SaveUsersToFile(users, "users.txt");
					loggedInUser = User::Login(login_, pass, users);
					cout << "Ви успішно зареєструвалися";
					system("cls");
				}
				else
				{
					SetCursor(6, 40);
					cout << "З поверненням \"" << login_ << "\"" << endl;
					SetCursor(7, 40);
					cout << "Введіть пароль для користувача " << login_ << ": ";
					cin >> pass;
					system("cls");
					auto userIterator = find_if(users.begin(), users.end(), [&login_](const User& user) {return user.getLogin() == login_; });
					if (userIterator != users.end()) {
						const string& storedHashedPassword = userIterator->getPassword();
						if (sha512(pass) == storedHashedPassword) {
							loggedInUser = &(*userIterator); 
						}
						else {
							cout << "Невірний пароль!" << endl;
						}
					}
					else {
						cout << "Користувач з логіном \"" << login_ << "\" не знайдений." << endl;
					}
				}

				if (loggedInUser)
				{
					SetCursor(4, 45);
					if (strcmp(loggedInUser->getLogin().c_str(), adminUser.getLogin().c_str()) == 0 && strcmp(loggedInUser->getPassword().c_str(), adminUser.getPassword().c_str()) == 0)
					{
						cout << "Ви увійшли в систему як " << loggedInUser->getName() << endl;
						bool exitMenu = false;
						while (!exitMenu)
						{
							select = 0;
							select = Menu(textAdminMenu, sizeAdminMenu, select);
							system("cls");
							switch (select)
							{
							case 0:
							{
								bool exitPidMenu = false;
								while (!exitPidMenu) 
								{
									select = Menu(textAdminPidMenuUser, sizeAdminPidMenuUser, select);
									system("cls");
									switch (select)
									{
									case 0:
										adminUser.addUser(users);
										loggedInUser = &adminUser;
										SaveUsersToFile(users, "users.txt");
										break;
									case 1:
										adminUser.removeUser(users, login_);
										SaveUsersToFile(users, "users.txt");
										break;
									case 2:
									{
										system("cls");
										SetCursor(20, 0);
										adminUser.modifyUser(users);
										SaveUsersToFile(users, "users.txt");
										break;
									}
									case 3:
									{
										system("cls");
										SetCursor(20, 0);
										adminUser.ViewAllUsers(users);
										break;
									}
									case 4:
									{
										SaveUsersToFile(users, "users.txt");
										break;
									}
									case 5:
										system("cls");
										exitPidMenu = true;
										select = 0;
										break;
									}
									if (loggedInUser == nullptr)
										system("cls");
									select = 0;
									break;
								}
								break;
							}
							case 1:
							{
								bool exitPidMenu = false;
								while (!exitPidMenu)
								{
									select = Menu(textAdminPidMenuStatistic, sizeAdminPidMenuStatistic, select);
									system("cls");
									switch (select)
									{
									case 0:
										LoadResultsFromFile("results.txt", testResults);
										adminUser.viewResultsByCategory(cat, testResults);
										break;

									case 1:
									{
										LoadResultsFromFile("results.txt", testResults);
										adminUser.viewResultsByTest(cat, testResults);
										break;
									}
									case 2:
									{
										LoadResultsFromFile("results.txt", testResults);
										adminUser.viewResultsByUsers(users, testResults);
										break;
									}
									case 3:
										exitPidMenu = true;
										break;
									}
								}
							}
							case 2:
							{
								bool exitPidMenu = false;
								while (!exitPidMenu) 
								{
									select = 0;
									select = Menu(textAdminPidMenuTest, sizeAdminPidMenuTest, select);
									system("cls");
									switch (select)
									{
									case 0:
									{
										system("cls");
										adminUser.addCategory(cat);
										adminUser.saveDataToFile(cat, "category.txt");
										SetCursor(20, 0);
										break;
									}
									case 1:
									{
										system("cls");
										adminUser.addTestToCategory(cat);
										adminUser.saveDataToFile(cat, "category.txt");
										SetCursor(20, 0);
										break;
									}
									case 2:
									{
										system("cls");
										SetCursor(20, 0);
										adminUser.addQuestionToTest(cat);
										adminUser.saveDataToFile(cat, "category.txt");
										break;
									}
									case 3:
									{
										adminUser.removeCategory(cat);
										adminUser.saveDataToFile(cat, "category.txt");
										break;
									}
									case 4:
									{
										adminUser.removeTestFromCategory(cat);
										adminUser.saveDataToFile(cat, "category.txt");
										break;
									}
									case 5:
									{
										system("cls");
										SetCursor(20, 0);
										for (const auto& item : cat)
										{
											cout << item << "\n";
										}
										break;
									}
									case 6:
									{
										SetCursor(20, 0);
										adminUser.printAllTestsInCategory(cat);
										break;
									}
									case 7:
									{
										adminUser.viewQuestionsAndAnswersInTest(cat);
										break;
									}
									case 8:
									{
										adminUser.saveDataToFile(cat, "category.txt");
										break;
									}
									case 9:
									{
										adminUser.importDataFromFile(cat, "category.txt");
										break;
									}
									case 10:
										exitPidMenu = true;
										break;
									}

								}
								break;
							}
							case 3:
							{
								cout << "Введіть новий пароль: ";
								cin.ignore();
								cin >> temp2;
								adminUser.ChangePassword(sha512(temp2));
								loggedInUser->ChangePassword(sha512(temp2));
								cout << "Пароль успішно оновлено." << endl;
								SaveUsersToFile(users, "users.txt");
								break;
							}
							case 4:
								cout << "Користувач " << loggedInUser->GetName() << " вийшов з системи." << endl;
								loggedInUser = nullptr;
								loggedIn = true;
								exitMenu = true;
								select = 0;
								break;
							}
						}
						

					}
					else
					{
						bool exitGuestMenu = false;
						while (!exitGuestMenu)
						{
							cout << "Ви увійшли в систему як " << loggedInUser->getName() << endl;
							select = Menu(textGuestMenu, sizeGuestMenu, select);
							system("cls");
							switch (select)
							{
							case 0:
								guest.takeTestInCategory(cat, users, loggedInUser->getLogin());
								guest.saveResultsToFile("results.txt", loggedInUser->getLogin());
								break;
							case 1:
								guest.loadResultsFromFile("results.txt", loggedInUser->getLogin());
								SetCursor(20, 0);

								guest.viewPreviousTestResults(loggedInUser->getLogin());
								break;
							case 2:
							{


								cout << "Введіть новий пароль: ";
								cin.ignore();
								cin >> temp2;
								loggedInUser->ChangePassword(sha512(temp2));
								cout << "Пароль успішно оновлено." << endl;
								SaveUsersToFile(users, "users.txt");
								break;
							}
							case 3:
								cout << "Користувач " << loggedInUser->GetName() << " вийшов з системи." << endl;
								loggedInUser = nullptr;
								loggedIn = true;
								exitGuestMenu = true;
								break;
							}
						}
					}
					system("cls");
					select = 0;
				}
				else
				{
					cout << "Неправильний логін або пароль." << endl;
				}

			}
			break;
		}
		case 2:
			return 0;
		default:
			break;
		}
	}
}
