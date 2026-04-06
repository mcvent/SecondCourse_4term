#pragma once
#include "Matrix.h"
#include "Transform.h"
#include "MyForm.h"
#include "Figure.h"
#include <vector>
#include <fstream>
#include <sstream>

namespace Burov {

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
	vector<path> figure;    // контейнер для хранения всех ломаных
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
			//
			//TODO: добавьте код конструктора
			//
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
			this->btnOpen->Location = System::Drawing::Point(394, 12);
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
			this->ClientSize = System::Drawing::Size(481, 334);
			this->Controls->Add(this->btnOpen);
			this->DoubleBuffered = true;
			this->KeyPreview = true;
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
		g->Clear(Color::Cyan);

		//проходим по всем ломаным в рисунке
		for (int i = 0; i < figure.size(); i++) {
			path lines = figure[i];
			Pen^ pen = gcnew Pen(Color::FromArgb((int)lines.color.x, (int)lines.color.y, (int)lines.color.z));
			pen->Width = lines.thickness;
			if (lines.vertices.size() == 0) continue;

			//преобразуем первую точку и сохраняем как начало первого отрезка
			vec2 start = normalize(T * vec3(lines.vertices[0], 1.0f));

			//проходим по остальным точкам
			for (int j = 1; j < lines.vertices.size(); j++) {
				//преобразуем текущую точку
				vec2 end = normalize(T * vec3(lines.vertices[j], 1.0f));

				//рисуем отрезок
				g->DrawLine(pen, start.x, start.y, end.x, end.y);
				start = end;
			}
		}
	}
	private: System::Void MyForm_Resize(System::Object^ sender, System::EventArgs^ e) {
		Refresh();
	}
	private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {

	}
	private: System::Void MyForm_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
		//координаты центра окна
		float Wcx = ClientRectangle.Width / 2.f;
		float Wcy = ClientRectangle.Height / 2.f;

		switch (e->KeyCode) {
		case Keys::Escape:
			T = initT;
			break;

		case Keys::Q:
			T = translate(-Wcx, -Wcy) * T;
			T = rotate(0.01f) * T;
			T = translate(Wcx, Wcy) * T;
			break;

		case Keys::W:
			T = translate(0.f, -1.f) * T;
			break;

		case Keys::E:
			// Поворот по часовой стрелке на 0.01 радиан
			T = translate(-Wcx, -Wcy) * T;
			T = rotate(-0.01f) * T;  // отрицательный угол = по часовой стрелке
			T = translate(Wcx, Wcy) * T;
			break;

		case Keys::S:
			// Сдвиг вниз на 1 пиксел
			T = translate(0.f, 1.f) * T;  // положительный сдвиг по Y = вниз
			break;

		case Keys::A:
			// Сдвиг влево на 1 пиксел
			T = translate(-1.f, 0.f) * T;  // отрицательный сдвиг по X = влево
			break;

		case Keys::D:
			// Сдвиг вправо на 1 пиксел
			T = translate(1.f, 0.f) * T;  // положительный сдвиг по X = вправо
			break;

		case Keys::R:
			// Поворот по часовой стрелке на 0.05 радиан
			T = translate(-Wcx, -Wcy) * T;
			T = rotate(-0.05f) * T;
			T = translate(Wcx, Wcy) * T;
			break;

		case Keys::Y:
			// Поворот против часовой стрелки на 0.05 радиан
			T = translate(-Wcx, -Wcy) * T;
			T = rotate(0.05f) * T;
			T = translate(Wcx, Wcy) * T;
			break;

		case Keys::T:
			// Сдвиг вверх на 10 пикселов
			T = translate(0.f, -10.f) * T;  // отрицательный Y = вверх
			break;

		case Keys::G:
			// Сдвиг вниз на 10 пикселов
			T = translate(0.f, 10.f) * T;   // положительный Y = вниз
			break;

		case Keys::F:
			// Сдвиг влево на 10 пикселов
			T = translate(-10.f, 0.f) * T;  // отрицательный X = влево
			break;

		case Keys::H:
			// Сдвиг вправо на 10 пикселов
			T = translate(10.f, 0.f) * T;   // положительный X = вправо
			break;

		case Keys::Z:
			// увеличение в 1.1 раза относительно центра окна
			T = translate(-Wcx, -Wcy) * T;   // перенос начала координат в центр окна
			T = scale(1.1f) * T;             // увеличение в 1.1 раза
			T = translate(Wcx, Wcy) * T;     // перенос начала координат обратно
			break;

		case Keys::X:
			// уменьшение в 1.1 раза относительно центра окна
			T = translate(-Wcx, -Wcy) * T;   // перенос начала координат в центр окна
			T = scale(1 / 1.1f) * T;           // уменьшение в 1.1 раза (коэффициент 0.90909...)
			T = translate(Wcx, Wcy) * T;     // перенос начала координат обратно
			break;

		case Keys::U:
			// зеркальное отражение относительно горизонтальной оси (Ox)
			T = translate(-Wcx, -Wcy) * T;   // перенос начала координат в центр окна
			T = mirrorX() * T;                // отражение относительно горизонтальной оси
			T = translate(Wcx, Wcy) * T;     // перенос начала координат обратно
			break;

		case Keys::J:
			// зеркальное отражение относительно вертикальной оси (Oy)
			T = translate(-Wcx, -Wcy) * T;   // перенос начала координат в центр окна
			T = mirrorY() * T;                // отражение относительно вертикальной оси
			T = translate(Wcx, Wcy) * T;     // перенос начала координат обратно
			break;

		case Keys::I:
			// растяжение по горизонтали в 1.1 раза (X увеличивается, Y не меняется)
			T = translate(-Wcx, -Wcy) * T;   // перенос начала координат в центр окна
			T = scale(1.1f, 1.0f) * T;       // растяжение по X в 1.1 раза, по Y без изменений
			T = translate(Wcx, Wcy) * T;     // перенос начала координат обратно
			break;

		case Keys::K:
			// сжатие по горизонтали в 1.1 раза (X уменьшается, Y не меняется)
			T = translate(-Wcx, -Wcy) * T;   // перенос начала координат в центр окна
			T = scale(1 / 1.1f, 1.0f) * T;     // сжатие по X в 1.1 раза, по Y без изменений
			T = translate(Wcx, Wcy) * T;     // перенос начала координат обратно
			break;

		case Keys::O:
			// растяжение по вертикали в 1.1 раза (Y увеличивается, X не меняется)
			T = translate(-Wcx, -Wcy) * T;   // перенос начала координат в центр окна
			T = scale(1.0f, 1.1f) * T;       // растяжение по Y в 1.1 раза, по X без изменений
			T = translate(Wcx, Wcy) * T;     // перенос начала координат обратно
			break;

		case Keys::L:
			// сжатие по вертикали в 1.1 раза (Y уменьшается, X не меняется)
			T = translate(-Wcx, -Wcy) * T;   // перенос начала координат в центр окна
			T = scale(1.0f, 1 / 1.1f) * T;     // сжатие по Y в 1.1 раза, по X без изменений
			T = translate(Wcx, Wcy) * T;     // перенос начала координат обратно
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
			wchar_t fileName[1024];
			for (int i = 0; i < openFileDialog->FileName->Length; i++)
				fileName[i] = openFileDialog->FileName[i];
			fileName[openFileDialog->FileName->Length] = '\0';


			ifstream in;
			in.open(fileName);
			if (in.is_open()) {
				// файл успешно открыт
				figure.clear(); // очищаем имеющийся список ломаных

				// временные переменные для чтения из файла
				float thickness = 2; // толщина со значением по умолчанию 2
				float r, g, b; // составляющие цвета
				r = g = b = 0; // значение составляющих цвета по умолчанию (черный)
				string cmd; // строка для считывания имени команды

				// непосредственно работа с файлом
				string str; // строка, в которую считываем строки файла
				getline(in, str); // считываем из входного файла первую строку
				while (in) { // если очередная строка считана успешно
					// обрабатываем строку
					if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
						// прочитанная строка не пуста и не комментарий
						stringstream s(str); // строковый поток из строки str
						s >> cmd;

						if (cmd == "frame") { // размеры изображения
							s >> Vx >> Vy; // считываем глобальные значения Vx и Vy
							aspectFig = Vx / Vy; // обновление соотношения сторон

							// === НОВЫЙ БЛОК: Вычисляем initT прямо здесь ===
							float Wx = ClientRectangle.Width; // размер окна по горизонтали
							float Wy = ClientRectangle.Height; // размер окна по вертикали
							float aspectForm = Wx / Wy; // соотношение сторон окна рисования
							// коэффициент увеличения при сохранении исходного соотношения сторон
							float S = aspectFig < aspectForm ? Wy / Vy : Wx / Vx;
							float Ty = S * Vy; // смещение в положительную сторону по оси Oy после смены знака
							initT = translate(0.f, Ty) * scale(S, -S); // совмещаем масштабирование и перенос
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

							string str1; // дополнительная строка для чтения из файла
							while (N > 0) { // пока не все точки считали
								getline(in, str1); // считываем в str1 из исходного файла очередную строку
								// так как файл корректный, то на конец файла проверять не нужно
								if ((str1.find_first_not_of(" \t\r\n") != string::npos) && (str1[0] != '#')) {
									// прочитанная строка не пуста и не комментарий
									// значит в ней пара координат
									float x, y; // переменные для считывания
									stringstream s1(str1); // еще один строковый поток из строки str1
									s1 >> x >> y;
									vertices.push_back(vec2(x, y)); // добавляем точку в список
									N--; // уменьшаем счетчик после успешного считывания точки
								}
							}
							// все точки считаны, генерируем ломаную (path) и кладем ее в список figure
							figure.push_back(path(vertices, vec3(r, g, b), thickness));
						}	
					}
					// считываем очередную строку
					getline(in, str);
				}
				Refresh(); // Обновляем форму после загрузки
			}
		}
	}
	};
}