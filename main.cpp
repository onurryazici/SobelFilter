#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
using namespace std;
///////  Onur YAZICI 
//
//      -- Bu açıklamayı okumak, anlamak adına daha yararlı olacaktır.
//
//      -- NOT : Çıktıları daha iyi görmek için Notepad++ yazılımını kullanarak
//      fontları küçük hale getirip daha iyi görebilirsiniz..
//
//		-- Gönderilmesi gereken parametreleri (command arguments) sırası ile şu şekilde belirleyin..
//		'image1.bin image2.bin fileout.txt'
//
//		-- Program ilk açılışta kullanılan operator overloading işlemleri yardımıyla
//		createImage metodu ile resmin binary verilerini 2 boyutlu matrise atar
//
//		-- Sobel metodunun çağırılmasıyla sobel filtreleme işlemi yapılır
//
//		-- Threshold metoduna gönderiler integer parametre ile degerler
//		istenen orana görene degistirilir
//
//		-- Yapılan and or not işlemleri ile iki resimlerin birleşimi, tersi vb.
//		işlemler yapılır
//
//		-- Program çalıştıktan sonra şu dosyalar yazılı bir şekilde çıkacaktır
//
//		and.txt				--> And işlemi yapıldıktan sonra oluşan görüntü
//		or.txt				--> Or işlemi yapıldıktan sorna oluşan görüntü
//		not.txt				--> Not işlemi yapıldıktan sonra oluşan görüntü
//		threshold.txt		--> Threshold sınırlaması ile oluşan görüntü
//		sobel.txt			--> Sobel matrsinin görüntüsü
//		binaries.txt		--> Resmin binary verileri
//		[3.parametre].txt	--> Gonderdiginiz üçüncü parametreye resmin son çıktısının görüntüsü
///////
string outputFileName;
class Image {
public:
	string fileName;
	int width;
	int height;
	int** binaries;								// Dosyanın içinde bulunan her 1 bayt veri burada saklanır..
	int** sobelValues;							// Gx ve Gy matrisinin hesaplanması bu dizide saklanır..
	void sobel();								// Sobel gezdirme işlemi bu metot ile yapılır
	void threshold(int value);					// Filtre değeri için
	void createImage(int width, int height);	// Resim ilk olarak bu metotla matrislere yerleştirilir.

	// Operator overloading
	// OR
	Image operator + (const Image& image)
	{
		Image temp;
		temp.width = image.width;
		temp.height = image.height;
		ofstream orOutput;
		orOutput.open("or.txt");
		temp.sobelValues = this->sobelValues;
		for (int i = 0; i < height - 2; i++)
		{
			for (int j = 0; j < width - 2; j++)
			{
				if (this->sobelValues[i][j] || image.sobelValues[i][j])
				{
					sobelValues[i][j] = 1;
					orOutput << "1";
				}
				else
				{
					sobelValues[i][j] = 0;
					orOutput << "0";
				}
			}
			orOutput << endl;
		}
		orOutput.close();
		return temp;
	}
	Image operator * (const Image& image)
	{
		Image temp;
		temp.width = image.width;
		temp.height = image.height;
		ofstream andOutput;
		andOutput.open("and.txt");
		temp.sobelValues = this->sobelValues;
		for (int i = 0; i < height - 2; i++)
		{
			for (int j = 0; j < width - 2; j++)
			{
				if (this->sobelValues[i][j] && image.sobelValues[i][j])
				{
					temp.sobelValues[i][j] = 1;
					andOutput << "1";
				}
				else
				{
					temp.sobelValues[i][j] = 0;
					andOutput << "0";
				}

			}
			andOutput << endl;
		}
		andOutput.close();
		return temp;
	}

	Image operator !() const
	{
		Image temp;
		temp.width = this->width;
		temp.height = this->height;
		temp.sobelValues = this->sobelValues;
		ofstream notOutput;
		notOutput.open("not.txt");
		for (int i = 0; i < (height - 2); i++)
		{
			for (int j = 0; j < (width - 2); j++)
			{
				int value = this->sobelValues[i][j];
				if (value == 0)
				{
					temp.sobelValues[i][j] = 1;
					notOutput << "*";
				}
				else if (value == 1)
				{
					temp.sobelValues[i][j] = 0;
					notOutput << "-";
				}

			}
			notOutput << endl;
		}
		notOutput.close();
		return temp;
	}
};

//// Operator Overloading
fstream& operator >> (fstream& file, Image& image) //// Reading file [OK]
{
	file.open(image.fileName, ios::in | ios::binary);
	int loop = 0;
	if (file.good())
	{
		while (!file.eof())
		{
			loop++;
			unsigned char x;
			file >> x;
			if (loop == 1)
			{
				image.height = static_cast<int>(x);
			}
			else if (loop == 2)
			{
				image.width = static_cast<int>(x);
				break;
			}
		}
	}
	file.close();
	if (image.width > 255 || image.height > 255)
	{
		cout << "Resim maksimum 255x255 boyutunda olmalidir.. " << endl << endl;
		system("pause");
		exit(0);
	}
	else
	{
		image.createImage(image.width, image.height);
	}

	fstream nullFstream;
	return nullFstream;
}

fstream& operator << (fstream& file, const Image& image)
{
	file.open(outputFileName, ios::out | ios::app);
	for (int i = 0; i < (image.height); i++)
	{
		for (int j = 0; j < (image.width); j++)
		{
			if (image.sobelValues[i][j] == 0)
			{
				file << "-";
			}
			else if (image.sobelValues[i][j] == 1)
			{
				file << "*";
			}
		}
		file << endl;
	}
	file.close();
	return file;
}

//// Creating Image and Sobel Filtering
void Image::createImage(int width, int height) // Filling 'binaries' array
{
	// Array Format
	// 220 width
	// 200 height
	// [width][height]
	// Creating image matrix
	binaries = new int* [height + 30];
	for (int i = 0; i < width; i++)
	{
		binaries[i] = new int[width + 30];
	}

	char* mem;
	ifstream::pos_type size;
	ifstream file;
	ofstream binaryOutput;
	binaryOutput.open("binaries.txt");
	file.open(fileName, ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		mem = new char[size];
		file.seekg(0, ios::beg);
		file.read(mem, size);
		file.close();
		size_t i = 0;
		for (int j = 0; j < width; j++)
		{
			for (int k = 0; k < height; k++)
			{
				binaries[216][1] = 3;
				binaries[j][k] = (int)((unsigned char)mem[i]);
				binaryOutput << binaries[j][k] << "\t";
				i++;
			}
			binaryOutput << endl;
		}
		binaryOutput.close();
		// Eğer binary verilerini görmek isterseniz bu döngüyü açın.
		// Yardımcı program olarak Binary Viewer yazılımını kullanıp binary
		// verilerini kıyaslayabilirsiniz.
		/*int loop = 0;
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				cout << loop << " - " << "[" << j << "]" << "[" << k << "]" << " "<< binaries[j][k] << endl;
				loop++;
			}
		}*/
	}
	else
	{
		cout << fileName << " dosyasi bulunamadi.." << endl;
		system("pause");
		exit(0);
	}
	file.close();
}

void Image::sobel() // Sobel filtering [OK]
{
	// Sobel değerleri için bir dizi
	sobelValues = new int* [width];
	for (int i = 0; i < (height); i++)
	{
		sobelValues[i] = new int[height];
	}
	const int GxDefault[3][3] = { {-1, 0, 1}, {-2, 0,  2}, {-1, 0,  1} };
	const int GyDefault[3][3] = { {-1,-2,-1}, { 0, 0,  0}, { 1, 2,  1} };

	ofstream sobelOutput;
	sobelOutput.open("sobel.txt");
	int GxValue = 0;
	int GyValue = 0;
	int sobelPixel = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			GxValue = 0;
			GyValue = 0;
			if (y == 0 || x >= height - 1)
			{
				sobelPixel = 0;
			}
			else if (x == 0 || y >= width - 1)
			{
				sobelPixel = 0;
			}
			else
			{
				// Calculating GxValue
				for (int i = -1; i <= 1; i++)
				{
					for (int j = -1; j <= 1; j++)
					{
						GxValue += (GxDefault[j + 1][i + 1] * (int)(binaries[x + j][y + i]));
					}
				}
				// Calculating GyValue
				for (int i = -1; i <= 1; i++)
				{
					for (int j = -1; j <= 1; j++)
					{
						GyValue += (GyDefault[j + 1][i + 1] * (int)(binaries[x + j][y + i]));
					}
				}
				sobelPixel = sqrt(pow((double)GxValue, 2) + pow((double)GyValue, 2));
			}
			sobelValues[y][x] = sobelPixel;
			sobelOutput << sobelValues[y][x] << "\t";
		}
		sobelOutput << endl;
	}
	sobelOutput.close();
}
void Image::threshold(int value) // Threasholding [OK]
{
	ofstream file;
	file.open("threshold.txt");
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (sobelValues[i][j] < value)
			{
				file << "-";
				sobelValues[i][j] = 0;
			}
			else
			{
				file << "*";
				sobelValues[i][j] = 1;
			}
		}
		file << endl;
	}
	file.close();
}

int main(int argc, char* argv[])
{

	Image image1, image2;
	image1.fileName = argv[0];
	image2.fileName = argv[1];
	outputFileName = argv[2];
	fstream file1, file2;

	file1 >> image1;
	file2 >> image2;

	image1.sobel();
	image2.sobel();

	image1.threshold(100);
	image2.threshold(100);

	Image last;
	last = image1 + image2;
	last = image1 * image2;
	last = !image1;

	fstream file3;
	file3 << last;
	system("pause");
}
