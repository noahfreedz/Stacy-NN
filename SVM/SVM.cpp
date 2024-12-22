#include "SVM.h"
#include <ostream>
#include <sstream>
#include <vector>


using namespace std;
namespace stacy {
    class MNSTData {
    public:
        vector<double> data;
        int lable;
    };

    class SVMData {
        vector<double> W; // this is the weights or the direction of the hyperplain
        int B; // this is the Bais or the offest of the hyperplain from the origain
    };

    class SVMMulti {
        double dotProduct(const std::vector<double>& v1, const std::vector<double>& v2) {
            if (v1.size() != v2.size()) {
                throw std::invalid_argument("Vectors must have same size");
            }

            double sum = 0.0;
            for (size_t i = 0; i < v1.size(); ++i) {
                sum += v1[i] * v2[i];
            }
            return sum;
        }

    };

    class SVM: private SVMMulti{
    public:
        SVM();

    private:
        int C; // this the cost for getting things wrong
        int L; // this is the learning rate

    };



}
