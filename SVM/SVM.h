#include <iostream>
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

    class SVMData
    {
     public:
        vector<double> W; // this is the weights or the direction of the hyperplain
        double B; // this is the Bais or the offest of the hyperplain from the origain
    };

    class SVMMulti {
    public:
        SVMMulti(const double learningRate, const double Cost);
        double predictOne(const vector<double>& image, const vector<double>& weights, double bias);
        double calculateNorm(const vector<double>& weights);
        void train(const vector<MNSTData>& MNSTData, vector<SVMData>& SVMData, const int num_epochs );
        private:
         double C;
         double L;
    };

        class SVM{
        public:
            double calculateNorm(const vector<double> &weights);

            SVM(const double cost, const double learningRate);
            double predictOne(const vector<double>& image, const vector<double>& weights, double bias);
            void train(const vector<stacy::MNSTData> &MNSTData, int TargetNumber, SVMData &currentClass);

        private:
            double C; // this the cost for getting things wrong
            double L; // this is the learning rate

        };
}
