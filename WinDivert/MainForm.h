#include <Windows.h>

#pragma once

namespace WinDivert {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::Button^  bStart;
	public: System::Windows::Forms::Button^  bStop;
	protected:

	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->bStart = (gcnew System::Windows::Forms::Button());
			this->bStop = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// bStart
			// 
			this->bStart->Location = System::Drawing::Point(75, 64);
			this->bStart->Name = L"bStart";
			this->bStart->Size = System::Drawing::Size(75, 23);
			this->bStart->TabIndex = 0;
			this->bStart->Text = L"Start";
			this->bStart->UseVisualStyleBackColor = true;
			this->bStart->Click += gcnew System::EventHandler(this, &MainForm::bStart_Click);
			// 
			// bStop
			// 
			this->bStop->Enabled = false;
			this->bStop->Location = System::Drawing::Point(75, 127);
			this->bStop->Name = L"bStop";
			this->bStop->Size = System::Drawing::Size(75, 23);
			this->bStop->TabIndex = 1;
			this->bStop->Text = L"Stop";
			this->bStop->UseVisualStyleBackColor = true;
			this->bStop->Click += gcnew System::EventHandler(this, &MainForm::bStop_Click);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 261);
			this->Controls->Add(this->bStop);
			this->Controls->Add(this->bStart);
			this->Name = L"MainForm";
			this->Text = L"MainForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	public: System::Void bStart_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void bStop_Click(System::Object^  sender, System::EventArgs^  e);
	};
}
