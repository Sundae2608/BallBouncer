#pragma once

#include "../single/single.h"

namespace backend {
    class SingleFormation {
        public:
            SingleFormation();
            ~SingleFormation();
            void Resize(int new_rows, int new_cols);
            Single* Get(int i, int j);
            void Set(int i, int j, Single* single);
        private:
            // Number of rows and cols
            int rows_;
            int cols_;

            // Single formation, represented by 2d array with pointers
            Single*** formation_;
    };
}