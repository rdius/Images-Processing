



#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <string.h>


using namespace cv;
using namespace std;


Mat histogramme(Mat image)
{
	//declaration des Variables
	vector<Mat> recupRGB;
	split( image, recupRGB);
	int tailleHisto = 256;
	float intervalle[] = { 0, 256 } ;
	const float* intervalleHisto = { intervalle };
	int largeurHisto = 512; int hauteurHisto = 500;
	int pas = 2;
	Mat histImage(hauteurHisto, largeurHisto, CV_8UC3, Scalar( 0,0,0));
	Mat HistRouge,HistVert,HistBleu;

	//Calcul de l'histogramme et niveau RGB
	calcHist( &recupRGB[0], 1, 0, Mat(),HistBleu , 1, &tailleHisto, &intervalleHisto, true, false);
	normalize(HistBleu,HistBleu, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	calcHist( &recupRGB[1], 1, 0, Mat(), HistVert, 1, &tailleHisto, &intervalleHisto, true, false);
	normalize(HistVert,HistVert, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	calcHist( &recupRGB[2], 1, 0, Mat(), HistRouge, 1, &tailleHisto, &intervalleHisto, true, false);
	normalize(HistRouge,HistRouge, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	//Tracage de l'histogramme en rectangle
	for( int i = 0; i < tailleHisto; i++ )
	{
		rectangle(histImage, Point( pas*(i), hauteurHisto) ,Point( pas*(i+1), hauteurHisto - cvRound(HistBleu.at<float>(i)) ),Scalar( 255, 0, 0), 2, 8, 0 );

		rectangle(histImage, Point( pas*(i), hauteurHisto) ,Point( pas*(i+1), hauteurHisto - cvRound(HistVert.at<float>(i)) ),Scalar( 0, 255, 0), 2, 8, 0 );

		rectangle(histImage, Point( pas*(i), hauteurHisto) ,Point( pas*(i+1), hauteurHisto - cvRound(HistRouge.at<float>(i)) ),Scalar( 0, 0, 255), 2, 8, 0 );
	}
return histImage;
}


// Fonction de calcul du profil d'intensite
Mat ProfilIntensiteImage(Mat image, int type, int numero)
{
	Point premierPoint, deuxiemePoint;
    int largeur_fenetre_courbe;
    Vec3b valeurPixelDeb;
    Vec3b valeurPixelFin;
	// Profil d'intensité dans le cas d'une ligne horizontale
	if(type==1){
		largeur_fenetre_courbe = image.cols;
	}
	else if(type==2){
		largeur_fenetre_courbe = image.rows;
	}
		// Image du profil

		Mat courbe_profil (256,largeur_fenetre_courbe,CV_8UC3,Scalar(0,0,0));

		for(int i = 0; i < largeur_fenetre_courbe-1; i++)
		{    //Recuperation des valeurs du pixel
			if(type==1){
		    valeurPixelDeb  = image.at<Vec3b>(numero, i);
		    valeurPixelFin = image.at<Vec3b>(numero,i+1);
			}
			else if(type==2){
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
		return courbe_profil;
	}

//Tracage du profil d'intensité sur l'image
Mat tracerTrait(Mat image, int type, int numero){

	Point debut, fin;

	if (type==1){

		debut.x = 0;
		debut.y = numero;
		fin.x = image.cols;
		fin.y = numero;
	}
	else if(type==2){
		debut.x = numero;
		debut.y = 0;
		fin.x = numero;
		fin.y = image.rows;
	}

	//Tracé de la ligne sur l'image
	line(image, debut, fin, Scalar(0, 0, 255), 1, 8);

	return image;
}


// Fonction pour modifier le contraste avec la fonction linéaire
Mat fonctionLineaire3Points(Mat image,Point P1, Point P2, Point P3){

	Mat sortie(image.rows, image.cols, CV_8UC3, Scalar( 0,0,0));
    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            for(int c = 0; c < 3; c++){
					int val = image.at<Vec3b>(i,j)[c];
					if(val>=0 && val <= P1.x) val = (int) P1.y/P1.x * val;
    				else if(P1.x < val && val <= P2.x) val =(int) ((P2.y - P1.y)/(P2.x - P1.x)) * (val - P1.x) + P1.y;
    				else if(P2.x < val && val <= P3.x) val =(int) ((P3.y - P2.y)/(P3.x - P2.x)) * (val - P2.x) + P2.y;
    				else if(P3.x < val && val <= 255) val =(int) ((255 - P3.y)/(255 - P3.x)) * (val - P3.x) + P3.y;
					sortie.at<Vec3b>(i,j)[c] = saturate_cast<uchar>(val);
            }
        }
    }
    return sortie;
}

// Fonction pour représenter la fonction linéaire
Mat courbeFonctionLineaire3Points(Point Point1, Point Point2, Point Point3){

	// Declaration de l'image de la courbe
	Mat courbe3Points(256,256,CV_8UC3,Scalar(0,0,0));

	// Déclaration et initialisation des points de début et de fin
	Point debut, fin;
	debut.x = 0 ;
	debut.y = 255 ;
	fin.x = 255;
	fin.y = 0;
	Point1.y = 255 - Point1.y;
	Point2.y = 255 - Point2.y;
	Point3.y = 255 - Point3.y;

	// Tracé des lignes de la courbe
	line(courbe3Points, debut, Point1, Scalar(255, 0, 0), 1, 8);
	line(courbe3Points, Point1, Point2, Scalar(255, 0, 0), 1, 8);
	line(courbe3Points, Point2, Point3, Scalar(255, 0, 0), 1, 8);
	line(courbe3Points, Point3, fin, Scalar(255, 0, 0), 1, 8);

	return courbe3Points;
}

// Fonction pour modifier le contraste suivant la correction de gamma
Mat correctionGamma(Mat image,float gamma){

	Mat sortie(image.rows, image.cols, CV_8UC3, Scalar( 0,0,0));

    for(int i = 0; i < image.rows; i++){

        for(int j = 0; j < image.cols; j++){

            for(int c = 0; c < 3; c++){

					int val = image.at<Vec3b>(i,j)[c];

					if(val < 0) val = 0;
    				else if(val > 255) val =255;
    				else if(0 <= val && val <= 255)
						val = pow((val/255.0),gamma)*255.0;
               sortie.at<Vec3b>(i,j)[c] = saturate_cast<uchar>(val);
            }
        }
    }
    return sortie;
}


// Fonction pour afficher la courbe de gamma
Mat courbeFonctionGamma(float gamma){

	Mat sortie(256, 256, CV_8UC3, Scalar( 0,0,0));

	Point Point1, Point2;
	for (float i = 0.0; i<256.0; i++){
		Point1.x = i;
		Point2.x = i+1;
		Point1.y = 255.0 - (pow((Point1.x/255.0),gamma)*255.0);
		Point2.y = 255.0 - (pow((Point2.x/255.0),gamma)*255.0);
		if(Point1.y < 0.0)  Point1.y = 0.0;
		if(Point1.y> 255.0) Point1.y = 255.0;
		if(Point2.y < 0.0)  Point2.y = 0.0;
		if(Point2.y> 255.0) Point2.y = 255.0;
		line(sortie, Point1, Point2, Scalar(255, 0, 0), 1, 8);
	}
	return sortie;
}

// Fonction pour modifier le constraste suivant les points min et max
Mat fonctionLineaireSaturation(Mat image, Point P1, Point P2){

	Mat sortie(image.rows, image.cols, CV_8UC3, Scalar( 0,0,0));

    for(int i = 0; i < image.rows; i++){

        for(int j = 0; j < image.cols; j++){

            for(int c = 0; c < 3; c++){

					int val = image.at<Vec3b>(i,j)[c];

					if(val>=0 && val <= P1.x) val = 0;
    				else if(P1.x < val && val <= P2.x)
						val =(int) (255/(P2.x - P1.x)) * (val - P1.x);
    				else if(P2.x < val && val <= 255) val = 255;
               sortie.at<Vec3b>(i,j)[c] = saturate_cast<uchar>(val);
            }
        }
    }
    return sortie;
}

// Fonction pour représenter la fonction linéaire
Mat courbeFonctionLineaireSaturation(Point Point1, Point Point2){

	// Declaration de l'image de la courbe
	Mat courbeSaturation(256,256,CV_8UC3,Scalar(0,0,0));

	// Déclaration et initialisation des points de début et de fin
	Point debut, fin;
	debut.x = 0 ;
	debut.y = 255 ;
	fin.x = 255;
	fin.y = 0;
	Point1.y = 255 - Point1.y;
	Point2.y = 255 - Point2.y;


	// Tracé des lignes de la courbe
	line(courbeSaturation, debut, Point1, Scalar(255, 0, 0), 1, 8);
	line(courbeSaturation, Point1, Point2, Scalar(255, 0, 0), 1, 8);
	line(courbeSaturation, Point2, fin, Scalar(255, 0, 0), 1, 8);

	return courbeSaturation;
}


int main( int argc, char** argv )
{
	int choix=0;
	char nomImage[100];
	Mat image_orig, histo_orig, profil_orig, trace_orig;
	Mat image_modi, histo_modi, profil_modi, trace_modi;
	Mat courbe;
	int type;
	int numero;

	if(argc!=1){
		cout << "Veuillez donner seulement le nom ./contraste"<< endl;
	}
	else
	{
		cout << "Modifier le contraste de votre image"<<endl;
		do{
			cout << "1-- Fonction linéaire à trois points"<<endl;
			cout << "2-- Correction gamma"<<endl;
			cout << "3-- Fonction linéaire avec saturation"<<endl;
			cout << "4-- Exit"<<endl;
			cout << "Veuillez faire un choix: $ ";
			cin  >> choix;

			if((choix !=1)&&(choix!=2)&&(choix!=3)&&(choix!=4)){
				cout << "Veuillez choisir un numero valide"<<endl;
			}
			else{
				if(choix == 4){
					cout << "Bye !!!"<< endl;
					return 0;
				}
				else{
					cout << "Donner le nom de l'image"<<endl;
					cout << "$ ";
					cin  >> nomImage;
					image_orig = imread(nomImage, 1 ); // chargement de l'image
					if(!image_orig.data){
						cout << "Nom invalide"<<endl;
					}
					else{

						cout << "Choisisir 1 pour profil ligne, 2 pour profil colonne" << endl;
						cin >> type;
						cout << "Donner le numero de la ligne ou de la colonne choisie"<<endl;
						cin >> numero;
						switch (choix)
						{
							case 1: // Modification fonction linéaire à deux points
							{
								//int abscisse1, ordonnee1, abscisse2, ordonnee2;
								Point Point1, Point2, Point3;
								// Collecte des informations spécifiques à la fonction linéaires
								cout << "Fonction linéaire à trois points"<<endl;
								cout << " Entrer les coordonnées"<<endl;
								cout << "Premier Point"<<endl;
								cout << "Abscisse : $ ";
								cin  >> Point1.x;
								cout << "Ordonnée : $ ";
								cin  >> Point1.y;

								cout << "Deuxieme Point"<<endl;
								cout << "Abscisse : $ ";
								cin  >> Point2.x;
								cout << "Ordonnée : $ ";
								cin  >> Point2.y;

								cout << "Troisieme Point"<<endl;
								cout << "Abscisse : $ ";
								cin  >> Point3.x;
								cout << "Ordonnée : $ ";
								cin  >> Point3.y;

								// Traitement de l'image selon les données receuillies
								image_modi = fonctionLineaire3Points(image_orig,Point1, Point2, Point3);
								courbe = courbeFonctionLineaire3Points(Point1, Point2, Point3);

								// Affichage de la courbe de la fonction
								imshow( "Fonction linéaire à trois points", courbe );

								// Enregistrement de l'histogramme orginal dans un fichier image
								if(!imwrite("fonction_lineaire.png", courbe))
									cout<<"Erreur lors de l'enregistrement"<<endl;
								break;
							}
							case 2: // Modification avec correction gamma
							{
								float gamma;
								// Collecte des informations spécifiques à la fonction linéaires
								cout << "Modification avec la correction gamma"<<endl;
								cout << " Donner la valeur de gamma"<<endl;
								cin  >> gamma;

								// Traitement de l'image selon la valeur de gamma
								image_modi = correctionGamma(image_orig,gamma);
								courbe = courbeFonctionGamma(gamma);

								// Affichage de la courbe de la fonction
								imshow( "Fonction gamma", courbe );

								// Enregistrement de l'histogramme orginal dans un fichier image
								if(!imwrite("fonction_gamma.png", courbe))
									cout<<"Erreur lors de l'enregistrement"<<endl;
								break;
							}
							case 3: // Modification linaire avec saturation
							{
								Point Point1, Point2;
								// Collecte des informations spécifiques à la fonction linéaires
								cout << "Fonction linéaire de saturation"<<endl;
								cout << " Entrer la valeur min et max"<<endl;

								cout << "Le minimum $ ";
								cin  >> Point1.x;
								Point1.y = 0;

								cout << "Le maximum $ ";
								cin  >> Point2.x;
								Point2.y = 255;

								// Traitement de l'image selon min et max
								image_modi = fonctionLineaireSaturation(image_orig,Point1, Point2);
								courbe = courbeFonctionLineaireSaturation(Point1, Point2);

								// Affichage de la courbe de la fonction
								imshow( "Fonction lineaire avec saturation", courbe );

								// Enregistrement de l'histogramme orginal dans un fichier image
								if(!imwrite("fonction_saturation.png", courbe))
									cout<<"Erreur lors de l'enregistrement"<<endl;
								break;
							}
						}

						// ************** Affichabe donnée originale *******************/
						profil_orig = ProfilIntensiteImage(image_orig, type, numero);

						trace_orig = tracerTrait(image_orig, type, numero);

						histo_orig = histogramme(image_orig);

						//Affichage de l'histogramme de image original
						imshow("Histogramme de l'image originale", histo_orig );

						// Enregistrement de l'histogramme orginal dans un fichier image
						if(!imwrite("histo_original.png", histo_orig))
							cout<<"Erreur lors de l'enregistrement"<<endl;

						// Affichage du profil original
						imshow( "profil d'Intensité original", profil_orig );

						// Enregistrement du profil original dans un fichier image
						if(!imwrite("profil_Intensite_original.png", profil_orig))
							cout<<"Erreur lors de l'enregistrement"<<endl;

						// Affichage de l'image originale avec le trait du profil
						imshow( "Image originale avec trait", trace_orig );

						// Enregistrement de l'image originale avec trait dans un fichier image
						if(!imwrite("image_originale_trait.png", trace_orig))
							cout<<"Erreur lors de l'enregistrement"<<endl;

						// ************* Affichage des données modifiées ***************/

						profil_modi = ProfilIntensiteImage(image_modi, type, numero);

						trace_modi = tracerTrait(image_modi, type, numero);

						histo_modi = histogramme(image_modi);

						//Affichage de l'histogramme modifié
						imshow("Histogramme de l'image modifiée", histo_modi );

						// Enregistrement de l'histogramme modifié dans un fichier image
						if(!imwrite("histo_modifie.png", histo_modi))
							cout<<"Erreur lors de l'enregistrement"<<endl;

						// Affichage du profl modifié
						imshow( "profil d'Intensité modifié", profil_modi );

						// Enregistrement du profil modifié dans un fichier image
						if(!imwrite("profil_Intensite_modifie.png", profil_modi))
							cout<<"Erreur lors de l'enregistrement"<<endl;

						// Affichage de l'image modifiée avec le trait du profil
						imshow( "Image Modifiee avec trait", trace_modi );

						// Enregistrement du l'image modifiée avec le trait du profil
						if(!imwrite("image_modifiee_trait.png", trace_modi))
							cout<<"Erreur lors de l'enregistrement"<<endl;

						/******** Action utilisateur ****************/
						// Attente d'une action de l'utilisateur
						waitKey(0);

						// Destruction des fenêtres
						cvDestroyAllWindows();

						// Quitter le programme
						return 0;
					}
				}
			}
		}while(choix!=4);
	}
}




