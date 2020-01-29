//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <Comobj.hpp>
#include "Excel.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
float Final;
unsigned char excel_bandera =0;
bool bandera = false;
bool Error = false;
bool pagina = false;
bool ok;
AnsiString Dato;
short int a=0;
unsigned char Dato_Recibido;
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Boton1Click(TObject *Sender)
{
if (bandera == false){
try{
 PuertoCom->Open();
 }catch(...) {
 MessageBox(0, "ERROR!! EL PUERTO YA ESTA ABIERTO O NO EXISTE!!!", "Firtec",
         MB_OK | MB_SYSTEMMODAL);
 Error = true;
 }
  if (Error == false){
 Boton1 -> Caption = "Cortar";
 bandera = true;
 Timer1 -> Enabled = true;
   }
 Error = false;

 }
 else
 {
  ok = false;
 Timer1 -> Enabled = false;
 Boton1 -> Caption = "Conectar";
 PuertoCom -> PurgeRead();
 PuertoCom -> PurgeReadWrite();
 PuertoCom-> Close();
 bandera = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PuertoChange(TObject *Sender)
{
Boton1 -> Enabled = true;
PuertoCom->PortNum = Puerto ->ItemIndex + 1;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  switch(excel_bandera){
  case 1: {
    excel_bandera = 2;
  //-------- hace EXCEL visible
        xlApp = CreateOleObject("Excel.Application");
        xlApp.Exec(PropertySet("Visible") << true);
     break;
  }
 case 3:{
 excel_bandera = 4;
//-------- crea acceso al objeto 'libro'/workbook
    xlBooks = xlApp.Exec(PropertyGet("Workbooks"));
//-------- Crea un Archivo nuevo
// ... o abre uno existente
     xlBooks.Exec(Procedure("Open") << "\\Controlador.xls");
    break;
 }
 case 4:{
 //-------- Tomo el primer libro de la aplicacion
try{
    xlBook = xlBooks.Exec(PropertyGet("Item") << 1);
}catch(...)  {

 Boton1 -> Caption = "Conectar";
 PuertoCom-> Close();
 bandera = false;


    MessageBox(0, "No puedo Conectarme con Controlador.XLS", "ERROR",
         MB_OK | MB_SYSTEMMODAL);
       Close();
       Timer1 -> Enabled = false;
       Boton1 -> Caption = "Conectar";
 PuertoCom -> PurgeRead();
 PuertoCom -> PurgeReadWrite();
 PuertoCom-> Close();
 bandera = false;

 }

    //-------- crea acceso al objeto 'hoja'/worksheets
    xlSheets = xlBook.Exec(PropertyGet("Worksheets"));

    //-------- Tomo la primera hoja del libro
    xlSheet = xlSheets.Exec(PropertyGet("Item") << 1);

    //-------- Trabajando con celdas...
    if(ok == false){

     VRange = xlSheet.Exec(PropertyGet("Range") << "A17");
    VRange.Exec(PropertySet("Value") << " www.firtec.com.ar");
    ok = true;
    }
    PuertoCom -> WriteChar(1); //Pido los datos al microcontrolador enviando un "1"
    //-------- Escribe texto en las primeras 3x3 celdas
    VRange = xlSheet.Exec(PropertyGet("Range") << "D6");
    VRange.Exec(PropertySet("Value") << a);
   }

 }        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PuertoComRxChar(TObject *Sender, int Count)
{
Byte convertir;
        Dato = PuertoCom -> ReadText();
        convertir = Dato[1];
        a= convertir;
        if(excel_bandera == 0){
        if(a==0x01){
        excel_bandera = 1;
     }
    }
        if(excel_bandera == 2){
        if(a== 0x02){
        excel_bandera = 3;
     }
   }


}
//---------------------------------------------------------------------------











