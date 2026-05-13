#pragma once
#include "Matrix.h"
#include "Transform.h"
#include "MyForm.h"
#include "Figure.h"
#include "Clip.h"
#include <vector>
#include <fstream>
#include <sstream>

namespace Akhmetov {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;

	float Vx;               // размер рисунка по горизонтали
	float Vy;               // размер рисунка по вертикали
	float aspectFig;        // соотношение сторон рисунка
	vector<model> models;
	mat3 T;                 // матрица, в которой накапливаются все преобразования
	mat3 initT;             // матрица начального преобразования

	/// <summary>
	/// Сводка для MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::OpenFileDialog^ openFileDialog;
	private: System::Windows::Forms::Button^ btnOpen;

	private: float left = 30, right = 100, top = 20, bottom = 50; // расстояния до границ окна

	private: float minX = left, maxX;
	private: float minY = top, maxY;
	private: float Wcx = left, Wcy;
	private: float Wx, Wy;

	private: System::Void rectCalc() {
		maxX = ClientRectangle.Width - right;
		maxY = ClientRectangle.Height - bottom;
		Wcy = maxY;
		Wx = maxX - left;
		Wy = maxY - top;
	}

	protected:

	protected:

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->openFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->btnOpen = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// openFileDialog
			// 
			this->openFileDialog->DefaultExt = L"txt";
			this->openFileDialog->FileName = L"Сделать поле пустым";
			this->openFileDialog->Filter = L"Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";
			this->openFileDialog->Title = L"Открыть файл";
			this->openFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::openFileDialog1_FileOk);
			// 
			// btnOpen
			// 
			this->btnOpen->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btnOpen->Location = System::Drawing::Point(1193, 12);
			this->btnOpen->Name = L"btnOpen";
			this->btnOpen->Size = System::Drawing::Size(75, 23);
			this->btnOpen->TabIndex = 0;
			this->btnOpen->Text = L"Открыть";
			this->btnOpen->UseVisualStyleBackColor = true;
			this->btnOpen->Click += gcnew System::EventHandler(this, &MyForm::btnOpen_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1280, 809);
			this->Controls->Add(this->btnOpen);
			this->DoubleBuffered = true;
			this->KeyPreview = true;
			this->MinimumSize = System::Drawing::Size(155, 120);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::MyForm_Paint);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MyForm::MyForm_KeyDown);
			this->Resize += gcnew System::EventHandler(this, &MyForm::MyForm_Resize);
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
	private: System::Void MyForm_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
		Graphics^ g = e->Graphics;
		g->Clear(Color::Aquamarine);

		Pen^ rectPen = gcnew Pen(Color::Black, 2);
		g->DrawRectangle(rectPen, left, top, Wx, Wy);

		for (int k = 0; k < models.size(); k++) {
			vector<path> figure = models[k].figure;
			mat3 TM = T * models[k].modelM;
			for (int i = 0; i < figure.size(); i++) {
				path lines = figure[i];
				Pen^ pen = gcnew Pen(Color::FromArgb((int)lines.color.x, (int)lines.color.y, (int)lines.color.z));
				pen->Width = lines.thickness;

				vec2 start = normalize(TM * vec3(lines.vertices[0], 1.0));
				for (int j = 1; j < lines.vertices.size(); j++) {
					vec2 end = normalize(TM * vec3(lines.vertices[j], 1.0));
					vec2 tmpEnd = end;
					if (clip(start, end, minX, minY, maxX, maxY)) {
						g->DrawLine(pen, start.x, start.y, end.x, end.y);
					}
					start = tmpEnd;
				}
			}
		}
	}
	private: System::Void MyForm_Resize(System::Object^ sender, System::EventArgs^ e) {
		rectCalc();
		Refresh();
	}
	private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {
		rectCalc();
	}
	private: System::Void MyForm_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
		//координаты центра окна
		float centerX = left + Wx / 2;
		float centerY = top + Wy / 2;

		switch (e->KeyCode) {
		case Keys::Escape:
			T = initT;
			break;

		case Keys::Q:
			T = translate(-centerX, -centerY) * T;
			T = rotate(0.01f) * T;
			T = translate(centerX, centerY) * T;
			break;

		case Keys::W:
			T = translate(0.f, -1.f) * T;
			break;

		case Keys::E:
			// поворот по часовой стрелке на 0.01 радиан
			T = translate(-centerX, -centerY) * T;
			T = rotate(-0.01f) * T;  // отрицательный угол = по часовой стрелке
			T = translate(centerX, centerY) * T;
			break;

		case Keys::S:
			// сдвиг вниз на 1 пиксел
			T = translate(0.f, 1.f) * T;
			break;

		case Keys::A:
			// сдвиг влево на 1 пиксел
			T = translate(-1.f, 0.f) * T;
			break;

		case Keys::D:
			// сдвиг вправо на 1 пиксел
			T = translate(1.f, 0.f) * T;
			break;

		case Keys::R:
			// поворот по часовой стрелке на 0.05 радиан
			T = translate(-centerX, -centerY) * T;
			T = rotate(-0.05f) * T;
			T = translate(centerX, centerY) * T;
			break;

		case Keys::Y:
			// поворот против часовой стрелки на 0.05 радиан
			T = translate(-centerX, -centerY) * T;
			T = rotate(0.05f) * T;
			T = translate(centerX, centerY) * T;
			break;

		case Keys::T:
			// сдвиг вверх на 10 пикселов
			T = translate(0.f, -10.f) * T;
			break;

		case Keys::G:
			// сдвиг вниз на 10 пикселов
			T = translate(0.f, 10.f) * T;
			break;

		case Keys::F:
			// сдвиг влево на 10 пикселов
			T = translate(-10.f, 0.f) * T;
			break;

		case Keys::H:
			// сдвиг вправо на 10 пикселов
			T = translate(10.f, 0.f) * T;
			break;

		case Keys::Z:
			// увеличение в 1.1 раза относительно центра окна
			T = translate(-centerX, -centerY) * T;   // перенос начала координат в центр окна
			T = scale(1.1f) * T;             // увеличение в 1.1 раза
			T = translate(centerX, centerY) * T;     // перенос начала координат обратно
			break;

		case Keys::X:
			// уменьшение в 1.1 раза относительно центра окна
			T = translate(-centerX, -centerY) * T;   // перенос начала координат в центр окна
			T = scale(1 / 1.1f) * T;           // уменьшение в 1.1 раза
			T = translate(centerX, centerY) * T;     // перенос начала координат обратно
			break;

		case Keys::U:
			// зеркальное отражение относительно Ox
			T = translate(-centerX, -centerY) * T;   // перенос начала координат в центр окна
			T = mirrorX() * T;                // отражение относительно горизонтальной оси
			T = translate(centerX, centerY) * T;     // перенос начала координат обратно
			break;

		case Keys::J:
			// зеркальное отражение относительно Oy
			T = translate(-centerX, -centerY) * T;   // перенос начала координат в центр окна
			T = mirrorY() * T;                // отражение относительно вертикальной оси
			T = translate(centerX, centerY) * T;     // перенос начала координат обратно
			break;

		case Keys::I:
			// растяжение по горизонтали в 1.1 раза
			T = translate(-centerX, -centerY) * T;   // перенос начала координат в центр окна
			T = scale(1.1f, 1.0f) * T;       // растяжение по X в 1.1 раза, по Y без изменений
			T = translate(centerX, centerY) * T;     // перенос начала координат обратно
			break;

		case Keys::K:
			// сжатие по горизонтали в 1.1 раза
			T = translate(-centerX, -centerY) * T;
			T = scale(1 / 1.1f, 1.0f) * T; // сжатие по X в 1.1 раза, по Y без изменений
			T = translate(centerX, centerY) * T;
			break;

		case Keys::O:
			// растяжение по вертикали в 1.1 раза
			T = translate(-centerX, -centerY) * T;
			T = scale(1.0f, 1.1f) * T; // растяжение по Y в 1.1 раза, по X без изменений
			T = translate(centerX, centerY) * T;
			break;

		case Keys::L:
			// сжатие по вертикали в 1.1 раза
			T = translate(-centerX, -centerY) * T;
			T = scale(1.0f, 1 / 1.1f) * T; // сжатие по Y в 1.1 раза, по X без изменений
			T = translate(centerX, centerY) * T;
			break;


		default:
			break;
		}
		Refresh();
	}
	private: System::Void openFileDialog1_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) {
	}
	private: System::Void btnOpen_Click(System::Object^ sender, System::EventArgs^ e) {
		if (openFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			wchar_t fileName[1024]; // переменная, в которой посимвольно сохраним имя файла
			for (int i = 0; i < openFileDialog->FileName->Length; i++)
				fileName[i] = openFileDialog->FileName[i];
			fileName[openFileDialog->FileName->Length] = '\0';

			// объявление и открытие файла
			ifstream in;
			in.open(fileName);
			if (in.is_open()) {
				// файл успешно открыт
				models.clear(); // очищаем имеющийся список рисунков
				mat3 M = mat3(1.f); // матрица для получения модельной матрицы
				mat3 initM; // матрица для начального преобразования каждого рисунка
				vector<mat3> transforms; // стек матриц преобразований
				vector<path> figure; // список ломаных очередного рисунка
				float thickness = 2;
				float r, g, b;
				r = g = b = 0;
				string cmd; // строка для считывания имени команды
				
				string str; // строка, в которую считываем строки файла
				getline(in, str);
				while (in) { // если очередная строка считана успешно
					// обрабатываем строку
					if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {

						stringstream s(str); // строковый поток из строки str
						s >> cmd;
						if (cmd == "frame") {
							s >> Vx >> Vy; // считываем глобальные значение Vx и Vy
							aspectFig = Vx / Vy; // обновление соотношения сторон
							float aspectRect = Wx / Wy; // соотношение сторон прямоугольника
							// смещение центра рисунка с началом координат
							mat3 T1 = translate(-Vx / 2, -Vy / 2);

							// коэффициент увеличения при сохранении исходного соотношения сторон
							float S = aspectFig < aspectRect ? Wy / Vy : Wx / Vx;
							mat3 S1 = scale(S, -S);
							// сдвиг точки привязки из начала координат в нужную позицию
							mat3 T2 = translate(Wx / 2 + Wcx, Wcy - Wy / 2);

							initT = T2 * (S1 * T1);
							T = initT;
						}
						else if (cmd == "color") { // цвет линии
							s >> r >> g >> b; // считываем три составляющие цвета
						}
						else if (cmd == "thickness") { // толщина линии
							s >> thickness; // считываем значение толщины
						}
						else if (cmd == "path") { // набор точек
							vector<vec2> vertices; // список точек ломаной
							int N; // количество точек
							s >> N;
							string str1;
							while (N > 0) {
								getline(in, str1); // считываем в str1 из входного файла очередную строку

								if ((str1.find_first_not_of(" \t\r\n") != string::npos) && (str1[0] != '#')) {
									float x, y; // переменные для считывания
									stringstream s1(str1); // еще один строковый поток из строки str1
									s1 >> x >> y;
									vertices.push_back(vec2(x, y)); // добавляем точку в список
									N--; // уменьшаем счетчик после успешного считывания точки
								}
							}
							figure.push_back(path(vertices, vec3(r, g, b), thickness));
						}
						else if (cmd == "model") {
							float mVcx, mVcy, mVx, mVy; // параметры команды model
							s >> mVcx >> mVcy >> mVx >> mVy; // считываем значения переменных
							float S = mVx / mVy < 1 ? 2.f / mVy : 2.f / mVx;

							initM = scale(S) * translate(-mVcx, -mVcy);
							figure.clear();
						}
						else if (cmd == "figure") { // формирование новой модели
							models.push_back(model(figure, M * initM));
						}
						else if (cmd == "translate") {
							float Tx, Ty; // параметры преобразования переноса
							s >> Tx >> Ty; // считываем параметры
							M = translate(Tx, Ty) * M;
						}
						else if (cmd == "scale") {
							float S; // параметр масштабирования
							s >> S; // считываем параметр
							M = scale(S) * M;
						}
						else if (cmd == "rotate") {
							float theta; // угол поворота в градусах
							s >> theta; // считываем параметр
							M = rotate(-theta / 180.f * Math::PI) * M;
						}
						else if (cmd == "pushTransform") { // сохранение матрицы в стек
							transforms.push_back(M); // сохраняем матрицу в стек
						}
						else if (cmd == "popTransform") { // откат к матрице из стека
							M = transforms.back(); // получаем верхний элемент стека
							transforms.pop_back(); // выкидываем матрицу из стека
						}
					}
					// считываем очередную строку
					getline(in, str);
				}
				Refresh();
			}
		}
	}
	};
}