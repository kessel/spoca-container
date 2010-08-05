// This program will use tracked color maps to output a file for the 3D visualisation program of Tufan Colak
// Written by Benjamin Mampaey on 26 July 2010

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>

#include "../classes/tools.h"
#include "../classes/constants.h"
#include "../classes/mainutilities.h"
#include "../classes/ArgumentHelper.h"

#include "../classes/SunImage.h"
#include "../classes/Region.h"
#include "../classes/CoordinateConvertor.h"
#include "../classes/trackable.h"



using namespace std;
using namespace dsr;
using namespace cgt;

string outputFileName;


int main(int argc, const char **argv)
{

	// The list of names of the images to process
	string imageType = "AIA";
	vector<string> sunImagesFileNames;

	// Options for the output
	string algorithmId = "SPoCA";
	unsigned wavelength = 171;
	string coordinateType = "HGC";
	string oit;
	bool monochrome = false;


	string programDescription = "This Programm will output a file for the 3D visualisation program of Tufan Colak.\n";
	programDescription+="Compiled with options :";
	programDescription+="\nDEBUG: "+ itos(DEBUG);
	programDescription+="\nPixelType: " + string(typeid(PixelType).name());
	programDescription+="\nReal: " + string(typeid(Real).name());

	ArgumentHelper arguments;
	arguments.set_string_vector("fitsFileName1 fitsFileName2 ...", "\n\tThe name of the fits files containing the tracked color maps.\n\t", sunImagesFileNames);
	arguments.new_named_unsigned_int('w', "wavelength", "positive integer", "\n\tThe wavelength in Angstrom\n\t", wavelength);
	arguments.new_named_string('I', "imageType", "string", "\n\tThe type of the images.\n\tPossible values are : EIT, EUVI, AIA, SWAP\n\t", imageType);
	arguments.new_named_string('T', "OIT", "string", "\n\tThe triplet Observatory Instrument Type (should be 3 digits).\n\t", oit);
	arguments.new_named_string('C', "coordinateType", "string", "\n\tThe type of coordinates to output positions.\n\tPossible values are : HG, HGC, HPC, HPR, HCC, HCR\n\t", coordinateType);
	arguments.new_named_string('A', "algorithmId", "string", "\n\tThe id of the algorithm.\n\t", algorithmId);	
	arguments.new_flag('M', "monochrome", "Set this flag if you want the regions to be outputed in monochrome colour.\n\t", monochrome);
	arguments.new_named_string('O', "outputFile","file name", "\n\tThe name for the output file(s).\n\t", outputFileName);
	arguments.set_description(programDescription.c_str());
	arguments.set_author("Benjamin Mampaey, benjamin.mampaey@sidc.be");
	arguments.set_build_date(__DATE__);
	arguments.set_version("1.0");
	arguments.process(argc, argv);



	// General variables
	const string imageBegin = "IMAGE";
	const string imageEnd = "IMAGEEND";
	const string regionType = "AR";
	string colorTable;
	unsigned maxColors;
	float x, y;
	
	// We set the name of output files
	if(outputFileName.empty())
	{
		outputFileName = "./visualisation3D_" + algorithmId;
	}
	ofstream outputFile(outputFileName.c_str());
	

	if(monochrome)
	{
		colorTable = "C 1 0.0 0.0 1.0";
		maxColors = 1;
	}
	else
	{
		colorTable = "C   1  0.4517074  0.2651221  0.6277225\n C   2  0.6429157  0.3734724  0.8467278\n C   3  0.6494546  0.9507003  0.8225482\n C   4  0.3836346  0.1378416  0.2472993\n C   5  0.1320215  0.6261291  0.1266706\n C   6  0.3313836  0.9549958  0.9534312\n C   7  0.4038166  0.7657301  0.7469824\n C   8  0.5334817  0.9957995  0.6482908\n C   9  0.5641348  0.4364251  0.9065539\n C  10  0.5671093  0.6776864  0.3961572\n C  11  0.2255041  0.3512424  0.4203143\n C  12  0.8756708  0.4222727  0.2583487\n C  13  0.7726765  0.0471576  0.2719486\n C  14  0.4645935  0.7037408  0.2270123\n C  15  0.8403447  0.3178298  0.7962399\n C  16  0.7471141  0.5780976  0.8331688\n C  17  0.2520893  0.3863131  0.5309002\n C  18  0.4033866  0.2553830  0.3081606\n C  19  0.6146027  0.7872575  0.9276847\n C  20  0.3986335  0.3515259  0.6738418\n C  21  0.0678010  0.1799412  0.9699677\n C  22  0.4338353  0.2752106  0.0957641\n C  23  0.3590873  0.1060708  0.9936755\n C  24  0.1585280  0.7332158  0.5958386\n C  25  0.7095645  0.3928266  0.5405549\n C  26  0.0858829  0.0581147  0.9727129\n C  27  0.9668518  0.9462858  0.9332107\n C  28  0.7872637  0.6505496  0.4696661\n C  29  0.7181094  0.0201596  0.6398907\n C  30  0.1608582  0.8787382  0.0661198\n C  31  0.9851240  0.1353563  0.4136251\n C  32  0.3803262  0.8224964  0.9045819\n C  33  0.3627478  0.7035832  0.8121076\n C  34  0.0231547  0.0932033  0.7028108\n C  35  0.7986587  0.0570622  0.9788859\n C  36  0.1414839  0.9207168  0.1613032\n C  37  0.7720808  0.4226199  0.4877623\n C  38  0.5435718  0.6963161  0.5405200\n C  39  0.6943914  0.6366502  0.4675297\n C  40  0.7645857  0.2889344  0.5080448\n C  41  0.2226662  0.2368806  0.1261088\n C  42  0.2817786  0.0634277  0.9774820\n C  43  0.7060321  0.5921032  0.2594695\n C  44  0.1417858  0.6118830  0.5543535\n C  45  0.1799008  0.0461884  0.3695904\n C  46  0.2348306  0.5101810  0.0291620\n C  47  0.0437899  0.2463305  0.5978080\n C  48  0.5637318  0.8124781  0.5195079\n C  49  0.3587582  0.0769678  0.1288510\n C  50  0.8287359  0.4551406  0.2309979\n C  51  0.8519638  0.2217604  0.1366132\n C  52  0.1568859  0.5474651  0.6405990\n C  53  0.4783859  0.4758550  0.8042461\n C  54  0.2337031  0.3205691  0.8649330\n C  55  0.0585066  0.0923932  0.7807555\n C  56  0.3821065  0.8202806  0.7807211\n C  57  0.5923097  0.7613038  0.8469921\n C  58  0.2674498  0.3194400  0.8114040\n C  59  0.5737986  0.3964682  0.9483397\n C  60  0.4720748  0.9647512  0.5695499\n C  61  0.1140456  0.1228635  0.1493502\n C  62  0.8048168  0.5271681  0.8474891\n C  63  0.4036735  0.7647747  0.4409117\n C  64  0.5788146  0.9534807  0.4523683\n C  65  0.9297512  0.8523857  0.1018454\n C  66  0.1295692  0.6691910  0.7321984\n C  67  0.9518983  0.7872291  0.0931588\n C  68  0.5517696  0.9953740  0.5991843\n C  69  0.9558970  0.7778636  0.4569483\n C  70  0.9590244  0.1057976  0.5187122\n C  71  0.7192897  0.7771630  0.1410232\n C  72  0.9964072  0.2527417  0.5405381\n C  73  0.3844977  0.7159194  0.4909152\n C  74  0.4250879  0.1197403  0.6902879\n C  75  0.1843290  0.5571485  0.4385223\n C  76  0.1769940  0.9996070  0.1516040\n C  77  0.9735914  0.3718239  0.7484251\n C  78  0.3332693  0.8248067  0.7455136\n C  79  0.1102410  0.2016001  0.4844661\n C  80  0.0175330  0.6497348  0.2514312\n C  81  0.0079990  0.3834966  0.2449684\n C  82  0.8326775  0.6405953  0.4275504\n C  83  0.6769918  0.6151471  0.8296606\n C  84  0.5282202  0.6686103  0.4856305\n C  85  0.6676613  0.0592038  0.0281164\n C  86  0.7393002  0.4853707  0.0387335\n C  87  0.4710955  0.1682820  0.0923646\n C  88  0.2826432  0.9176015  0.3222606\n C  89  0.8213031  0.1393871  0.2441322\n C  90  0.9078674  0.0212734  0.7024487\n C  91  0.2930921  0.2725068  0.8404081\n C  92  0.2725228  0.5130567  0.6796013\n C  93  0.9942566  0.1303925  0.2902579\n C  94  0.0801162  0.6396307  0.7979931\n C  95  0.5535362  0.3955696  0.4182131\n C  96  0.8693690  0.2343380  0.3648880\n C  97  0.5067597  0.9731973  0.6395800\n C  98  0.6871225  0.5264259  0.3888202\n C  99  0.2996233  0.7352926  0.0546896\n C 100  0.3161368  0.5426472  0.6414768\n C 101  0.3227304  0.5966303  0.9922734\n C 102  0.9008343  0.5486544  0.5661443\n C 103  0.3277079  0.6787482  0.1301506\n C 104  0.4211778  0.7693352  0.2575102\n C 105  0.8182884  0.9435246  0.7386619\n C 106  0.2582727  0.4791131  0.3118299\n C 107  0.3496911  0.3075736  0.5722251\n C 108  0.7387941  0.5027140  0.4416496\n C 109  0.3382574  0.9134385  0.6718413\n C 110  0.1611725  0.9243553  0.3832795\n C 111  0.8046260  0.4684739  0.8314416\n C 112  0.4546362  0.3470031  0.3868515\n C 113  0.6815590  0.7788110  0.3022835\n C 114  0.8122365  0.9613463  0.0711312\n C 115  0.0625148  0.4142605  0.9553342\n C 116  0.5919209  0.2581961  0.1136571\n C 117  0.6255194  0.1873433  0.4766601\n C 118  0.2341627  0.9909744  0.2593350\n C 119  0.7581919  0.0533631  0.7890520\n C 120  0.1405124  0.8254867  0.0804231\n C 121  0.4769486  0.7090825  0.0420706\n C 122  0.9036147  0.6426516  0.2499877\n C 123  0.5503200  0.0458582  0.9138238\n C 124  0.2264343  0.5141602  0.3611490\n C 125  0.0289690  0.6366420  0.1090328\n C 126  0.9313530  0.4590166  0.6708531\n C 127  0.2523385  0.0769256  0.0346565\n C 128  0.3110287  0.8742275  0.2169329\n C 129  0.1043415  0.6046594  0.7210841\n C 130  0.5027882  0.5152223  0.0640618\n C 131  0.5372260  0.4893979  0.4720302\n C 132  0.2783691  0.2662306  0.0651392\n C 133  0.0545519  0.1563822  0.5438632\n C 134  0.8536490  0.5940396  0.8409200\n C 135  0.6074870  0.3924745  0.8825639\n C 136  0.6373600  0.0218126  0.7157745\n C 137  0.4114901  0.0755979  0.5310686\n C 138  0.6860864  0.0969397  0.9707765\n C 139  0.2288378  0.6912903  0.0951128\n C 140  0.0711210  0.0683642  0.0743614\n C 141  0.7915453  0.0519386  0.6701173\n C 142  0.7650426  0.8887311  0.9084750\n C 143  0.5097336  0.9976060  0.3300721\n C 144  0.0381617  0.7617326  0.3658343\n C 145  0.3414479  0.0249942  0.8554825\n C 146  0.7356692  0.3433132  0.1979357\n C 147  0.0232624  0.7058585  0.3363604\n C 148  0.5547259  0.7190894  0.0165737\n C 149  0.6186076  0.2974600  0.3162131\n C 150  0.6158712  0.4031836  0.4095263\n C 151  0.5613178  0.9471359  0.5743279\n C 152  0.7297411  0.5122751  0.8504740\n C 153  0.3633020  0.3145831  0.9116451\n C 154  0.5142411  0.8268511  0.3186349\n C 155  0.7592465  0.5327134  0.8914104\n C 156  0.5775281  0.0711881  0.5010071\n C 157  0.2417836  0.5215884  0.4328381\n C 158  0.3349135  0.2046670  0.2787897\n C 159  0.7947005  0.4259988  0.9414229\n C 160  0.9351503  0.6567646  0.4408500\n C 161  0.0919318  0.1410384  0.9317908\n C 162  0.9278816  0.1924123  0.0333112\n C 163  0.0770829  0.8392850  0.6569408\n C 164  0.9055319  0.2291559  0.7089563\n C 165  0.9383957  0.5492907  0.0007276\n C 166  0.8071282  0.2391984  0.4757724\n C 167  0.5490013  0.9728742  0.9162269\n C 168  0.8729725  0.8861579  0.7768475\n C 169  0.8381627  0.9883289  0.0159133\n C 170  0.0962483  0.2089650  0.0492768\n C 171  0.1958806  0.5611297  0.6374139\n C 172  0.7402782  0.4579536  0.2746974\n C 173  0.8620955  0.2561073  0.5912622\n C 174  0.4231654  0.2075058  0.8818469\n C 175  0.3950653  0.0250690  0.7905151\n C 176  0.6444948  0.1402471  0.8624497\n C 177  0.3435403  0.4942603  0.0556100\n C 178  0.9078885  0.4553612  0.5471287\n C 179  0.2014125  0.6289648  0.2853655\n C 180  0.3076047  0.3836946  0.3928883\n C 181  0.0279966  0.2745241  0.8825708\n C 182  0.0118042  0.7546933  0.8626517\n C 183  0.4744764  0.2209335  0.0248739\n C 184  0.2761674  0.1294224  0.1381254\n C 185  0.5046303  0.4282705  0.1580668\n C 186  0.6603629  0.8440993  0.6705660\n C 187  0.6277553  0.3865285  0.5393440\n C 188  0.2074657  0.8766220  0.2024916\n C 189  0.1817611  0.5228552  0.9044815\n C 190  0.8582841  0.6839370  0.9296280\n C 191  0.4107581  0.6210223  0.0196957\n C 192  0.5017019  0.0274879  0.8748794\n C 193  0.3803998  0.2546025  0.1039748\n C 194  0.1868008  0.6254553  0.2572889\n C 195  0.9774058  0.8101262  0.9266547\n C 196  0.7979109  0.1614612  0.7328506\n C 197  0.1331914  0.3214675  0.5252705\n C 198  0.0978520  0.2589833  0.5815302\n C 199  0.3675348  0.6554269  0.0205023\n C 200  0.5983618  0.4083343  0.4879757\n C 201  0.2035663  0.1797242  0.0504702\n C 202  0.2289172  0.3385198  0.2833896\n C 203  0.7545087  0.5450509  0.1046696\n C 204  0.5472928  0.4086539  0.8518074\n C 205  0.6680210  0.3799487  0.4287486\n C 206  0.6184300  0.9770840  0.7777213\n C 207  0.5980489  0.9009606  0.9887773\n C 208  0.4442322  0.6108888  0.1319423\n C 209  0.7632108  0.2099204  0.3482403\n C 210  0.3550614  0.9703954  0.0975037\n C 211  0.2458896  0.0113006  0.2500135\n C 212  0.3494939  0.4364769  0.6147975\n C 213  0.2202139  0.9447314  0.1230424\n C 214  0.1392244  0.1342966  0.4221936\n C 215  0.6893525  0.3058918  0.7050394\n C 216  0.2490652  0.6428658  0.5162284\n C 217  0.9283363  0.6784618  0.1335347\n C 218  0.8078910  0.9070060  0.7565945\n C 219  0.1221708  0.9421921  0.6450254\n C 220  0.8360378  0.5870060  0.9431401\n C 221  0.4003712  0.3171664  0.9294761\n C 222  0.7480469  0.2529162  0.0481297\n C 223  0.1555311  0.4906981  0.3273248\n C 224  0.9175407  0.1070089  0.7215684\n C 225  0.4994132  0.6415321  0.0391049\n C 226  0.6166536  0.9475006  0.5903981\n C 227  0.3579217  0.9162069  0.3477058\n C 228  0.0077144  0.0999017  0.8915335\n C 229  0.5545366  0.6672588  0.0846368\n C 230  0.1651091  0.8745853  0.9527062\n C 231  0.9274752  0.6553194  0.5186053\n C 232  0.2876038  0.0392544  0.1993084\n C 233  0.3253389  0.2304604  0.2478632\n C 234  0.2591771  0.6376282  0.3083755\n C 235  0.6265777  0.2229115  0.9894264\n C 236  0.8666710  0.7545097  0.4234974\n C 237  0.2243261  0.2527705  0.2887620\n C 238  0.4735650  0.5117224  0.8664620\n C 239  0.6957533  0.7485124  0.0030649\n C 240  0.6575897  0.2702234  0.6390029\n C 241  0.2072501  0.8079835  0.1631908\n C 242  0.2076965  0.8226080  0.2356374\n C 243  0.6483393  0.5341716  0.0104879\n C 244  0.3796101  0.8205460  0.5719783\n C 245  0.0759212  0.6449222  0.9890376\n C 246  0.3129467  0.7786196  0.5252741\n C 247  0.4898670  0.9084007  0.2647890\n C 248  0.4937546  0.1759516  0.5929419\n C 249  0.2600820  0.9046137  0.9883388\n C 250  0.7069743  0.7883911  0.5722969\n C 251  0.9361234  0.2192482  0.2400184\n C 252  0.6091005  0.7766958  0.8391533\n C 253  0.4307289  0.2812904  0.7207062\n C 254  0.4883905  0.8432181  0.1166695\n C 255  0.1096560  0.8129816  0.8592737\n C   256  0.2139458  0.8848081  0.4055839";
		maxColors = 256;
	}


	if(oit.empty())	//We try to guess it
	{
		if (imageType == "EIT")
			oit = "12";
		else if (imageType == "EUVI")
			oit = "23"; // For stereo-A
			// imagePrefix += " 33"; // For stereo-B
		else if (imageType == "AIA")
			oit = "66";
		else if (imageType == "SWAP")
			oit = "66";
		else 
			oit = "66";
	
		if(wavelength == 171)
			oit += "6";
		else if(wavelength == 195)
			oit += "7";
		else if(wavelength == 284)
			oit += "8";
		else if(wavelength == 304)
			oit += "9";
		else
			oit += "0";
	}

	//We output the regions for each image
	for (unsigned s = 0; s  < sunImagesFileNames.size(); ++s)
	{

		// We read the color maps
		SunImage* image = getImageFromFile(imageType, sunImagesFileNames[s]);

		// We crop the images
		image->preprocessing("NAR", 1);
	
		// We get the regions out of the images
		vector<Region*> regions = getRegions(image);	

		cout<<imageBegin<<" "<<oit<<regions[0]->Visu3DLabel()<<" "<<algorithmId<<endl;
		cout<<colorTable<<endl;
		
		// We will need to convert the coordinates
		CoordinateConvertor coco(image, coordinateType);
		
		for (unsigned r = 0; r < regions.size(); ++r)
		{
			coco.convert(regions[r]->Center(), x, y);
			cout<<regionType<<" "<<(regions[r]->Color() % maxColors) + 1<<" "<<x<<" "<<y<<" "<<regionType<<regions[r]->Color()<<endl;
			delete regions[r];
		}
		
		cout<<imageEnd<<endl;
	}


	return EXIT_SUCCESS;
}
