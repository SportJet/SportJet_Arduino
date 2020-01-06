
//---- include -----------------------------------------
#include <stdio.h>
#include <unistd.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

//---- prototypes --------------------------------------
void AfficherEtatDesBoutonsDuBoitier(unsigned char byteBoitier);
void AfficherChiffreSurAfficheur (int NomAff, int Val);
void AfficherChrono();
int main (void);


//--------- var globales -----------------------------------
int ScoreA = 0;
int ScoreB = 0;
int Min = -1;
int Sec = 0;
int CHR;
//C_Match LeMatch;
//C_Match LeMatch.run;


// ---- bus I2C --------
int fd;
int C1;
int C2;
int S1;
int S2;	

//---- fonctions ---------------------------------------
int main (void)
{
	
	//LeMatch.Init();

	//LeMatch.Run();





	int mode;
	int pinInputMode = 5 ;
	int numTour = 0;

	printf("---- Afficheur Test Boitier ----\n");
	printf(" (Boitier sur adresse I2C $20)\n");
	printf("   (Ctrl+C pour quitter)\n");

	// setup I2C
	fd = wiringPiI2CSetup(0x38);
	C1 = wiringPiI2CSetup(0x20);
	C2 = wiringPiI2CSetup(0x21);
	S1 = wiringPiI2CSetup(0x22);
	S2 = wiringPiI2CSetup(0x23);	

	// setup broche 4
	wiringPiSetup();
	pinMode(pinInputMode,INPUT);

	// RAZ Led

		wiringPiI2CWrite( C1,0x00);
		wiringPiI2CWrite( C2,0x00);
		wiringPiI2CWrite( S1,0x00);
		wiringPiI2CWrite( S2,0x00);

	if (fd>0)
	{
		printf(" Succes connexion \n");
		unsigned char res;
		int i;
		while (1)
		{
			printf(" ---> Boucle : %d ", numTour);
			numTour++;
			mode = digitalRead(pinInputMode);
			printf(" Mode : %i ", mode);
			if (mode == 1) 
				printf("-> MODE BOITIER <- ");
			else printf("   -> MODE WIFI <- ");
			
			// lecture des boutons sur bus I2C
			res = wiringPiI2CReadReg8(fd, 0x0);
			printf("    Res : %x\n", res);


			AfficherEtatDesBoutonsDuBoitier(res);
			
	
			
			
			// incrementation du chrono

			if ( CHR == 1)
			{

				AfficherChrono(Sec, Min);

				Sec = Sec + 1;
				if ( Sec >= 60)	// changerment de minute
				{
					Sec = 0;
					Min++;

				
				}
			}

			
			sleep(1);


		}
	}
	else printf(" error fd\n");
	printf("---- Fin du test ! ----\n");
	return 0;
}
//----------------------------------------------
void AfficherEtatDesBoutonsDuBoitier(unsigned char byteBoitier)
{
/*	int C1 = wiringPiI2CSetup(0x20);
	int C2 = wiringPiI2CSetup(0x21);
	int S1 = wiringPiI2CSetup(0x22);
	int S2 = wiringPiI2CSetup(0x23);
*/

	if ((byteBoitier & 0x01)!= 0x00)
	{
		printf ("Bouton CHRONO_RESET appuyé\n");
		
		wiringPiI2CWrite( C1,0x00);
		wiringPiI2CWrite( C2,0x00);
		wiringPiI2CWrite( S1,0x00);
		wiringPiI2CWrite( S2,0x00);

		CHR = 0;
		ScoreA = 0;
		ScoreB = 0;
		Sec = 0;
		Min = 0;

	}





	if ((byteBoitier & 0x02)!= 0x00)
	{
		printf ("Bouton CHRONO_STOP appuyé\n");
		CHR = 0;
	}




	if ((byteBoitier & 0x04)!= 0x00)
	{
		printf ("Bouton CHRONO_START appuyé\n");
		CHR = 1;
	}



	// bouton A- appuyé
	if ((byteBoitier & 0x08)!= 0x00)
	{
		printf ("Bouton SCORE_A- appuyé\n");
		if (ScoreA > 0)
		{
			ScoreA--;
		}
				
		AfficherChiffreSurAfficheur (3, ScoreA);

	}



	// bouton B- appuyé
	if ((byteBoitier & 0x10)!= 0x00)
	{
		printf ("Bouton SCORE_B- appuyé\n");
		if (ScoreB > 0)
		{
			ScoreB--;
		}
				
		AfficherChiffreSurAfficheur (4, ScoreB);
	}




	// bouton A+ appuyé
	if ((byteBoitier & 0x20)!= 0x00)
	{
		printf ("Bouton SCORE_A+ appuyé\n");
		ScoreA++;
		AfficherChiffreSurAfficheur (3, ScoreA);

	}


	// bouton B+ appuyé
	if ((byteBoitier & 0x40)!= 0x00)
	{
		printf ("Bouton SCORE_B+ appuyé\n");
		ScoreB++;
		AfficherChiffreSurAfficheur (4, ScoreB);
	}
}

//------------------------------------------------------------

void AfficherChiffreSurAfficheur (int NomAff, int Val)
{
	int nbDiz;
	int nbUnit;
	int Res;
	int octet = 0;

	printf (" ft afficher %i \n",Val);
	
	// Bouton A+ incrémentation	
	if (NomAff == 3)
	{
		//int S1 = wiringPiI2CSetup(0x22);

		if (Val < 10)
		{
			octet = Val;
			

		}

		if (Val >= 10)
		{
			nbDiz = Val/10;
			nbUnit = Val - (nbDiz *10);
			octet = (nbDiz *16) + nbUnit; 
			
		}

		wiringPiI2CWrite( S1, octet);
		}
		


	// Bouton B+ incrémentation
	if (NomAff == 4)
	{
		//int S2 = wiringPiI2CSetup(0x23);

		if (Val < 10)
		{
			octet = Val;
			

		}

		if (Val >= 10)
		{
			nbDiz = Val/10;
			nbUnit = Val - (nbDiz *10);
			octet = (nbDiz *16) + nbUnit; 
			
		}

		wiringPiI2CWrite( S2, octet);
		}



}


//------------------------------------------------------------

void AfficherChrono(int Seconde, int Minute)
{

	int nbDiz;
	int nbUnit;
	int octet = 0;

	// Seconde
		
		//int C2 = wiringPiI2CSetup(0x21);

			if (Seconde < 10)
			{
				octet = Seconde;
			

			}

			if (Seconde >= 10)
			{
				nbDiz = Seconde/10;
				nbUnit = Seconde - (nbDiz *10);
				octet = (nbDiz *16) + nbUnit; 
			
			}

			wiringPiI2CWrite( C2, octet);


	// Minute
		
		//int C1 = wiringPiI2CSetup(0x20);

			if (Seconde == 0)
			{
				Minute = Minute + 1 ;
				printf (" Min %i \n",Minute);

				if (Minute < 10)
				{
					octet = Minute;

					printf (" octet %i \n",octet);
			

				}

				if (Minute >= 10)
				{
					nbDiz = Minute/10;
					nbUnit = Minute - (nbDiz *10);
					octet = (nbDiz *16) + nbUnit; 
			
				}
				wiringPiI2CWrite( C1, octet);


			}




}







