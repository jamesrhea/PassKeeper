#include <owl/owlpch.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include "pass.rh"
#include "pass.hpp"
#include "passdlg.hpp"
#include "passcrpt.hpp"

#include <stdio.h>

TWelcomeDialog::TWelcomeDialog(TWindow *parent,TResId resId):TDialog(parent, resId)
{
 passwordEdit1 = new TEdit(this,IDE_NEWPASS1,MAX_ENTRY_SIZE);
 passwordEdit2 = new TEdit(this,IDE_NEWPASS2,MAX_ENTRY_SIZE);

}

TWelcomeDialog::~TWelcomeDialog()
{
	delete passwordEdit1;
	delete passwordEdit2;
}

bool
TWelcomeDialog::CanClose()
{
	char password2[MAX_ENTRY_SIZE];

	passwordEdit1->GetText(password,sizeof(password));
	passwordEdit2->GetText(password2,sizeof(password2));

	if (strlen(password) == 0 && strlen(password2)==0) {return false;}

	if (strlen(password2) == 0) {
		MessageBox("You must enter your password twice for confirmation");
		passwordEdit2->SetFocus();
		return false;
	}

	if (strcmp(password,password2) != 0) {
		MessageBox("The passwords do not match. Please re-enter them.");
		passwordEdit1->Clear();
		passwordEdit2->Clear();
		passwordEdit1->SetFocus();
		return false;
	}

	// pad out password if it is less than the crypt key size
	if (strlen(password) < CRYPT_KEY_SIZE-1) {
		int sum = 0;
		for (int i=0; i< strlen(password);i++) {
			sum += (int)password[i];
		}
		for (i=strlen(password);i<CRYPT_KEY_SIZE;i++) {
			password[i] = sum % 31 + 1;
		}
		password[CRYPT_KEY_SIZE]=0;
	}

	BYTE buffer[MAX_CRYPT_SIZE+IV_SIZE];
	memset(buffer,0,sizeof(buffer));
	memcpy(buffer,password,sizeof(password));

	TCrypt *crypt = new TCrypt();
	crypt->encrypt(buffer,sizeof(buffer),(BYTE *)password);
	delete crypt;

	ofstream datafile("pass.dat",ios::binary);
	datafile.write((char *)&buffer,sizeof(buffer));
	datafile.close();

	return TRUE;
}

//--------------------------------------------------------

TEnterPasswordDialog::TEnterPasswordDialog(TWindow *parent,TResId resId):TDialog(parent, resId)
{
	passwordEdit = new TEdit(this,IDE_ENTER_PASSWORD,MAX_ENTRY_SIZE);
	login_attempts = 0;
}

TEnterPasswordDialog::~TEnterPasswordDialog()
{
	delete passwordEdit;
}

bool
TEnterPasswordDialog::CanClose()
{
	BYTE encryptedPassword[MAX_CRYPT_SIZE+IV_SIZE];

	passwordEdit->GetText(password,sizeof(password));

  	// pad out password if it is less than the crypt key size
	if (strlen(password) < CRYPT_KEY_SIZE-1) {
		int sum=0;
		for (int i=0; i< strlen(password);i++) {
			sum += (int)password[i];
		}
		for (i=strlen(password);i<CRYPT_KEY_SIZE;i++) {
			password[i] = sum % 31 + 1; // keep extra chars in the range of control chars
		}
		password[CRYPT_KEY_SIZE]=0;
	}

	ifstream datafile("pass.dat",ios::binary);
	datafile.read(encryptedPassword,sizeof(encryptedPassword));
	datafile.close();

	BYTE buffer[MAX_ENTRY_SIZE+IV_SIZE];
	memset(buffer,0,sizeof(buffer));
	memcpy(buffer,encryptedPassword,sizeof(encryptedPassword));

	TCrypt *crypt = new TCrypt();
	crypt->decrypt(buffer,sizeof(buffer),(BYTE *)password);
	delete crypt;

	if (strcmp((char *)buffer,password) != 0) {
		MessageBox("Sorry, that password is incorrect.","Incorrect Password");
		if (++login_attempts > 2) CmCancel();

		passwordEdit->Clear();
		passwordEdit->SetFocus();

		return false;
	}

	return TRUE;
}

//--------------------------------------------------------


TChangePasswordDialog::TChangePasswordDialog(TWindow *parent,TResId resId):TDialog(parent, resId)
{
 passwordEdit1 = new TEdit(this,IDE_CHANGE_NEW1,MAX_ENTRY_SIZE);
 passwordEdit2 = new TEdit(this,IDE_CHANGE_NEW2,MAX_ENTRY_SIZE);
}

TChangePasswordDialog::~TChangePasswordDialog()
{
	delete passwordEdit1;
	delete passwordEdit2;
}

bool
TChangePasswordDialog::CanClose()
{
	char password2[MAX_ENTRY_SIZE];

	passwordEdit1->GetText(password,sizeof(password));
	passwordEdit2->GetText(password2,sizeof(password2));

	if (strlen(password) == 0 && strlen(password2)==0) {return false;}

	if (strlen(password2) == 0) {
		MessageBox("You must enter your password twice for confirmation");
		passwordEdit2->SetFocus();
		return false;
	}

	if (strcmp(password,password2) != 0) {
		MessageBox("The passwords do not match. Please re-enter them.");
		passwordEdit1->Clear();
		passwordEdit2->Clear();
		passwordEdit1->SetFocus();
		return false;
	}

  // pad out password if it is less than the crypt key size
	if (strlen(password) < CRYPT_KEY_SIZE-1) {
		int sum=0;
		for (int i=0; i< strlen(password);i++) {
			sum += (int)password[i];
		}
		for (i=strlen(password);i<CRYPT_KEY_SIZE;i++) {
			password[i] = sum % 31 + 1; // keep extra chars in the range of control chars
		}
		password[CRYPT_KEY_SIZE]=0;
	}

	return TRUE;
}