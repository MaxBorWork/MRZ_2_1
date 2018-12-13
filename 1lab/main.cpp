//@author Борисевич Максим, 621702
#include <iostream>
#include <fstream>

//@author CImg team, version 2.4.1
#include "CImg.h"

#define S 3
#define START_STEP_ERROR 100

int n, m, p;
int h, w;
int N, L;
double** W;
double** W2;
double** X;
double* Y;
double* X2;
double E = 0;
double Eq = 0;
double e;
double* a1 = 0;
double a2 = 0;

int main(int argc, char *argv[])
{
	char* filename;

	/*if (argc < 2) {
		return 1;
	}
	else {
		filename = argv[1];
	}*/
	cimg_library::CImg<unsigned char> image("bw.bmp");

	std::cout << "Enter n" << std::endl;
	std::cin >> n;
	std::cout << "Enter m" << std::endl;
	std::cin >> m;
	N = n * m * S;
	std::cout << "Enter p (<=" << N*2 << ") " << std::endl;
	std::cin >> p;
	std::cout << std::endl;
	std::cout << "Enter min error" << std::endl;
	std::cin >> e;
	std::cout << std::endl;

	int width = image.width();
	int height = image.height();
	w = width / n;
	h = height / m;
	L = w * h;

	//@author Гудилин Андрей, 621702
	char* str = new char[(N + L)* p * sizeof(double) + 3];
	std::ofstream file("commpressed", std::ios_base::binary | std::ios_base::trunc);
	file.write(str, (N + L) * p * sizeof(double) + 3);
	file.close();
	delete[]str;

	std::ifstream in("bw.bmp", std::ifstream::ate | std::ifstream::binary);
	double inSize = in.tellg();
	in.close();

	std::ifstream out("commpressed", std::ifstream::ate | std::ifstream::binary);
	double outSize = out.tellg();
	out.close();

	std::cout << "Z: " << inSize / outSize << std::endl;

	//@author Борисевич Максим, 621702
	W = new double*[N];
	W2 = new double*[p];

	for (int i = 0; i < N; i++) {
		W[i] = new double[p];
		for (int j = 0; j < p; j++) {
			W[i][j] = (((double)(rand()) / RAND_MAX * 2) - 1) * 0.1;
		}
	}
	for (int i = 0; i < p; i++) {
		W2[i] = new double[N];
		for (int j = 0; j < N; j++) {
			W2[i][j] = W[j][i];
		}
	}

	X = new double*[L];
	for (int i = 0; i < L; i++) {
		X[i] = new double[N];
	}

	int Li = -1;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int num = 0;
			Li++;
			for (int i1 = 0; i1 < n; i1++) {
				for (int j1 = 0; j1 < m; j1++) {
					for (int k = 0; k < S; k++) {
						X[Li][num++] = (2.0 * image(j*m + j1, i*n + i1, 0, k) / 255) - 1;
					}
				}
			}
		}
	}
	
	a1 = new double[L];
	for (int q = 0; q < L; q++) {
		a1[q] = START_STEP_ERROR;
		for (int i = 0; i < N; i++) {
			a1[q] += (X[q][i] * X[q][i]);
		}
		a1[q] = 1 / a1[q];
	}

	X2 = new double[N];
	Y = new double[p];
	double* ww = new double[p];

	int col_of_iter = 0;
	double oX;
	do
	{
		for (int q = 0; q < L; q++) {
			for (int i = 0; i < N; i++) {
				X2[i] = 0;
			}
			for (int i = 0; i < p; i++) {
				Y[i] = 0;
				ww[i] = 0;
			}
			for (int i = 0; i < p; i++) {
				for (int j = 0; j < N; j++) {
					Y[i] += W[j][i] * X[q][j];
				}
			}
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < p; j++) {
					X2[i] += W2[j][i] * Y[j];
				}
			}
			for (int i = 0; i < p; i++) {
				for (int j = 0; j < N; j++) {
					ww[i] += (X2[j] - X[q][j]) * W2[i][j];
				}
			}
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < p; j++) {
					W[i][j] -= (a1[q] * X[q][i] * ww[j]);
				}
			}
			a2 = START_STEP_ERROR;
			for (int i = 0; i < p; i++) {
				a2 += (Y[i] * Y[i]);
			}
			a2 = 1 / a2;
			for (int i = 0; i < p; i++) {
				for (int j = 0; j < N; j++) {
					W2[i][j] -= a2 * Y[i] * (X2[j] - X[q][j]);
				}
			}
		}
		
		E = 0;
		for (int q = 0; q < L; q++) {
			{
				Eq = 0;
				for (int i = 0; i < p; i++) {
					Y[i] = 0;
				}
				for (int i = 0; i < N; i++) {
					X2[i] = 0;
				}
				for (int i = 0; i < p; i++) {
					for (int j = 0; j < N; j++) {
						Y[i] += W[j][i] * X[q][j];
					}
				}
				for (int i = 0; i < N; i++) {
					for (int j = 0; j < p; j++) {
						X2[i] += W2[j][i] * Y[j];
					}
					oX = (X2[i] - X[q][i]);
					Eq += (oX * oX)/2;
				}
				E += Eq;
			}
		}
		col_of_iter++;
		std::cout << E << "\t";
	} while (E > e);

	cimg_library::CImg<unsigned char> result_image(width, height, 1, 3);
	result_image.fill(0);

	for (int q = 0; q < L; q++) {
		for (int i = 0; i < N; i++) {
			X2[i] = 0;
		}
		for (int i = 0; i < p; i++) {
			Y[i] = 0;
		}

		for (int i = 0; i < p; i++) {
			for (int j = 0; j < N; j++) {
				Y[i] += W[j][i] * X[q][j];
			}
		}
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < p; j++) {
				X2[i] += W2[j][i] * Y[j];
			}
		}
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				for (int k = 0; k < S; k++) {
					int pixel = (int)(255 * ((X2[(n * i + j)*S + k] + 1) / 2));
					if (pixel > 255) {
						pixel = 255;
					}
					if (pixel < 0) {
						pixel = 0;
					}
					result_image(q % w * m + j, q / h * n + i, 0, k) = pixel;
				}
			}
		}
	}
	result_image.save_bmp("result.bmp");

	std::cout << std::endl << std::endl;
	std::cout << "result E: " << E << std::endl;
	std::cout << "col of iterations: " << col_of_iter << std::endl;
	//double Z = (double) (N*L) / ((N + L) * p + 2);
	std::cout << "N: " << N << std::endl;
	std::cout << "L: " << L << std::endl;
	//std::cout << "coef of compression: " << inSize / outSize << std::endl;
	system("pause");
}
