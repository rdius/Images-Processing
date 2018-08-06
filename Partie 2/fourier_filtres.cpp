////////////////////////////////////////////////////////////////////////////////
//                      TP sur FOURRIER                                      //
//                  Partie 2:Traitements fréquentiels                       //
//                                                                         //
//                                                                        //
//               Réalisé par KAFANDO Rodrique IFI/P21                    //
//////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat OptimalSize (Mat I){

	    Mat padded;     //image redimensionnee  a renvoyer en sortie

	    // taille optimale de l image redimensionnee
	    int m = getOptimalDFTSize( I.rows );
	    int n = getOptimalDFTSize( I.cols );

	    //Ajout de valeur de bourrage dans l'image redimensionnee
	    copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
	    return padded;

}

Mat Fourier (Mat RedimSize){

	    // Creation de deux Conteneurs avec valeurs flottantes pour les parties reelles et imaginaires de la transformee de fourier
	    Mat planes[] = {Mat_<float>(RedimSize), Mat::zeros(RedimSize.size(), CV_32F)};

	    //Concatenation des deux conteneurs
	    Mat complexI;
	    merge(planes, 2, complexI);

	    //Transformee de fourier
	    dft(complexI, complexI);

	    return complexI;
}

Mat Norm (Mat complexI){


	    vector<Mat> planes;
	    // separation des parties reelles et imaginaires
	    split(complexI, planes);

	    // Calcul de norme de la transformee de fourier
	    magnitude(planes[0], planes[1], planes[0]);
	    Mat MagI = planes[0];

	    // Passage a l echelle logarithmique
	    MagI += Scalar::all(1);
	    log(MagI, MagI);


	    // recadrage du spectre
	   MagI = MagI(Rect(0, 0, MagI.cols & -2, MagI.rows & -2));
	    normalize(MagI,MagI, 0, 1, CV_MINMAX);
	    Mat Out;
	    MagI.convertTo(Out, CV_8UC1,255);

	    return Out;
}

Mat Newcadran(Mat MagI){
    int cx = MagI.cols/2;
    int cy = MagI.rows/2;

    Mat q0(MagI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(MagI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(MagI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(MagI, Rect(cx, cy, cx, cy)); // Bottom-Right

    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

    //normalize(MagI, MagI, 0, 1, NORM_MINMAX); // Transform the matrix with float values into a
                                            // viewable image form (float between values 0 and 1).
//Reconstruction de l'image par la dft inverse

	    return MagI;

}

Mat  Invertdft (Mat complexI, int n_rows, int n_cols){
	Mat ReconsImag;
	Mat InvertdftImag;
	vector<Mat> planes;

	//Transformee inverse de fourier
	//dft( ImageTransFourier,ImageFourierInverse,DFT_INVERSE + DFT_SCALE);
	dft(complexI, InvertdftImag,DFT_INVERSE+ DFT_SCALE);

	split(InvertdftImag,planes);

	ReconsImag = planes[0];

	//Recadrage de l image
	ReconsImag = ReconsImag(Rect(0, 0, ReconsImag.cols & -2, ReconsImag.rows & -2));

	//Conversion en image à niveaux de gris
	ReconsImag.convertTo(ReconsImag, CV_8UC1);
	Mat Out (ReconsImag, Rect(0,0, n_cols, n_rows));

	return Out;
}

Mat LowpassFilter (Mat complexI, float cut_freq ){
	    Mat LowpassImag;
	    vector<Mat> planes;
	    float cx = complexI.cols / (float) 2;
	    float cy = complexI.rows / (float) 2;
		// Determination du rayon du cercle
		int rayon = (int) (min(float(cx), float(cy)))*cut_freq;

		split(complexI,planes);

		// Mise à jour des valeurs à l'extérieur du cercle
		for(int i = 0;  i< (int) planes.size(); i++){
			Newcadran(planes[i]);
			for(int j = 0; j < planes[i].rows; j++)
				for(int w = 0; w< planes[i].cols; w++)
				{
					if(pow((w - cx),2) + pow((j - cy),2) > pow(rayon,2))
						planes[i].at<float>(j,w) = 0;
				}
		}
		merge(planes,LowpassImag);


		return LowpassImag;

}

Mat HightpassFilter (Mat complexI, float cut_freq ){
	    Mat HightpassImag;
	    vector<Mat> planes;
	   float cx = complexI.cols / (float) 2;
		float cy = complexI.rows / (float) 2;
		// Determination du rayon du cercle
		int rayon = (int) (min(float(cx), float(cy)))*cut_freq;

		split(complexI,planes);
		// Mise à jour des valeurs à l'extérieur du cercle
		for(int i = 0;  i< (int) planes.size(); i++){
			Newcadran(planes[i]);
			for(int j = 0; j < planes[i].rows; j++)
				for(int w = 0; w< planes[i].cols; w++)
				{
					if(pow((w - cx),2) + pow((j - cy),2)  < pow(rayon,2))
						planes[i].at<float>(j,w) = 0;
				}
		}
		merge(planes,HightpassImag);


		return HightpassImag;

}





int main( int argc, char** argv )
{
    int choix;
    float cut_freq;
	if(argc!= 2 ){
		cout << "Veuillez suivre la sytaxe suivante:./Filtres mon_image "<<endl;
	}
	else
	{
		cout << "Choix du filtre"<<endl;
		cout << "Tapez 1 pour le filtre passe-bas"<<endl;
		cout << "Tapez 2 pour le filtre passe-haut"<<endl;
		cout << "choix :";
        cin >> choix;
        cout << "Entrer la frequence de coupure 0 < fc <= 1:";
        cin >> cut_freq;
		const char* filename = argv[1];

		// Transforme l'image d'entrée en gris
		Mat I = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

		if(!I.data){
			cout << "Veuillez fournir une image valide"<<endl;
			return -1;
		}
		else{

			// Calcul de la transformée de fourier de l'image

			 Mat complexI = Fourier(OptimalSize(I));

			 Mat FourierImg;

			 //Application du filtre suivant le choix
			 if(choix == 1){
				 FourierImg = LowpassFilter(complexI, cut_freq);
			 }
			 else if(choix == 2){
				 FourierImg = HightpassFilter(complexI, cut_freq);
			 }

			 //Détermination de la norme du spectre
			 Mat Spectrum1 = Newcadran(Norm(complexI));
			 Mat EndSpectrum = Norm(FourierImg);

			 Mat InvertdftImag = Invertdft(Newcadran(FourierImg),I.rows, I.cols);

			 //Affichage des différentes images
			 imshow("Image Entree", I);
			 imshow("Spectre de Fourier Initial", Spectrum1);
			 imshow("Spectre de Fourier Traite", EndSpectrum);
			 imshow("Image Apres Traitement", InvertdftImag);

			 // Enregistrement 
			if(!imwrite("ImgOrignie.png", I))
			     cout<<"Erreur"<<endl;
			 if(!imwrite("SpectreFourier.png", Spectrum1))
			     cout<<"Erreur"<<endl;
			 if(!imwrite("Cut_Frq.png", EndSpectrum))
			     cout<<"Erreur"<<endl;
			 if(!imwrite("RestoredImg.png",InvertdftImag))
			     cout<<"Erreur"<<endl;

			 waitKey();

    		 return 0;

		}
	}
}

