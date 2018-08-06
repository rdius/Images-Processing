
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;



// fonction pour tracer la ligne de profil
void tracerTrait(Mat image, char* type, int numero){

	Point pointDebut, pointFin;
		// choisir 1 pour une 
	if (strcasecmp(type,"1")==0){

		pointDebut.x = 0;
		pointDebut.y = numero;
		pointFin.x = image.cols;
		pointFin.y = numero;
	}
	else if(strcasecmp(type,"2")==0){
		pointDebut.x = numero;
		pointDebut.y = 0;
		pointFin.x = numero;
		pointFin.y = image.rows;
	}

	//Tracer la ligne sur l'image
	line(image, pointDebut, pointFin, Scalar(0, 0, 255), 1, 8);

	//Affiche l'image 
	imshow("Image contenant la ligne du Profil",image);

	// Sauvegarde le résultat
	if(!imwrite("resultats/Image_Ligne.png", image)) cout<<"Error"<<endl;
}



// DETERMINER LE PROFIL D'INTENSITE D'UNE IMAGE 

void ProfilIntensiteImage(Mat image, char* type, int numero)
{
	Point premierPoint, deuxiemePoint;
    int largeur_fenetre_courbe;
    Vec3b valeurPixelDeb;
    Vec3b valeurPixelFin;
	// Profil d'intensité dans le cas d'une ligne horizontale
	if(strcasecmp(type,"1")==0){
		largeur_fenetre_courbe = image.cols;
	}
	else if(strcasecmp(type,"2")==0){
		largeur_fenetre_courbe = image.rows;
	}
		// Image du profil

		Mat courbe_profil (256,largeur_fenetre_courbe,CV_8UC3,Scalar(0,0,0));

		for(int i = 0; i < largeur_fenetre_courbe-1; i++)
		{    //Recuperation des valeurs du pixel
			if(strcasecmp(type,"1")==0){
		    valeurPixelDeb  = image.at<Vec3b>(numero, i);
		    valeurPixelFin = image.at<Vec3b>(numero,i+1);
			}
			else if(strcasecmp(type,"2")==0){
			valeurPixelDeb  = image.at<Vec3b>(i, numero);
			valeurPixelFin = image.at<Vec3b>(i+1,numero);
			}

			for(int j = 0; j < 3; j++)
			{
				premierPoint.x = i;
				deuxiemePoint.x = i+1;
				premierPoint.y = 255 - valeurPixelDeb.val[j];
				deuxiemePoint.y = 255 - valeurPixelFin.val[j];

				// Tracé du profil pour la couleur Bleu
				if(j==0) line(courbe_profil, premierPoint, deuxiemePoint, Scalar(255, 0, 0), 1, 8);
				

				// Tracé du profil pour la couleur Rouge
				if(j==1) line(courbe_profil, premierPoint, deuxiemePoint, Scalar(0, 255, 0), 1, 8);
				

				// Tracé du profil pour la couleur Verte
				if(j==2) line(courbe_profil, premierPoint, deuxiemePoint, Scalar(0, 0, 255), 1, 8);
			}

		}
		imshow( "profil d'Intensité", courbe_profil );
		// Enregistrement dans un fichier image
		if(!imwrite("resultats/profil_Int.png", courbe_profil))
			cout<<"Error"<<endl;
	}



// Main
int main( int argc, char** argv )
{
	Mat image_entree; // Variable pour stocker l'image
	int numero; // Le numero de la ligne ou de la colonne
	// charge l'image
	image_entree = imread( argv[1], 1 ); 

	if( argc != 4 )
	{
		cout << "Entrer les valeurs suivantes: nom_fichier type_profil numero_ligne_ou_colonne\n"<<endl;
	}
	else
	{
		if(!image_entree.data){
			cout<<"Nom incorrecte \n"<<endl;
			return 0;
		}
		else
		{
			if((strcasecmp(argv[2],"1")==0)||(strcasecmp(argv[2],"2")==0))
			{
				int test=0;
				numero = atoi(argv[3]);
				// Vérification du numero spécifié par l'utilisateur
				if((strcasecmp(argv[2],"1")==0)&&(image_entree.rows < numero)){
					cout<<"Entrer le numero de la ligne"<< image_entree.rows<<endl;
					test =1;
				}
				else if((strcasecmp(argv[2],"2")==0)&&(image_entree.cols < numero)){
					cout <<"Entrer le numero de la colonne"<<image_entree.cols<<endl;
					test = 1;
				}
				if(test==0)
				{
					// Ligne profil d'intensité
					ProfilIntensiteImage(image_entree, argv[2],numero);

					// Tracage
					tracerTrait(image_entree, argv[2],numero);

					waitKey(0);

					cvDestroyAllWindows();

					return 0;
				}
			}
			else
			{
			// syntax error
				cout<<"Syntaxe error\n"<<endl; 
				return 0;
			}
		}
	}
}




