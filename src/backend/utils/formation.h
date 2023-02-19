#pragma once

#include "../single/single.h"

namespace backend {
    namespace move_utils {
        class SingleFormation {
            public:
                SingleFormation() : rows_(0), cols_(0), formation_(nullptr) {}
                ~SingleFormation() {
                    if (data != nullptr) {
                        for (int i = 0; i < rows_; i++) {
                            delete [] formation_[i];
                        }
                        delete [] formation_;
                    }
                }

                // Resize the formation
                void Resize(int new_rows, int new_cols) {
                    // Delete old data
                    for (int i = 0; i < rows_; i++) {
                        delete [] formation_[i];
                    }
                    delete [] data;

                    // Create new data;
                    rows_ = new_rows;
                    cols_ = new_cols;
                    formation_ = new Single*[rows_];
                    for (int i = 0; i < rows; i++) {
                        formation_[i] = new Single[cols_];
                    }
                }

                // Get the single at certain location
                Single* Get(int i, int j) {
                    return formation_[i][j];
                }

                // Set the single at certain location
                void Set(int i, int j, Single* single) {
                    formation_[i][j] = single;
                }
            private:
                // Number of rows and cols
                int rows_;
                int cols_;

                // Single formation, represented by 2d array with pointers
                Single** formation_;   
        }
    }
}