#include "single_formation.h"

namespace backend {
    
    SingleFormation::SingleFormation() : rows_(0), cols_(0), formation_(nullptr) {}
    SingleFormation::~SingleFormation() {
        if (formation_ != nullptr) {
            for (int i = 0; i < rows_; i++) {
                delete [] formation_[i];
            }
            delete [] formation_;
        }
    }

    // Resize the formation
    void SingleFormation::Resize(int new_rows, int new_cols) {
        // Delete old data
        for (int i = 0; i < rows_; i++) {
            delete [] formation_[i];
        }
        delete [] formation_;

        // Create new data;
        rows_ = new_rows;
        cols_ = new_cols;
        formation_ = new Single**[rows_];
        for (int i = 0; i < rows_; i++) {
            formation_[i] = new Single*[cols_];
        }
    }

    // Get the single at certain location
    Single* SingleFormation::Get(int i, int j) {
        return formation_[i][j];
    }

    // Set the single at certain location
    void SingleFormation::Set(int i, int j, Single* single) {
        formation_[i][j] = single;
    }
}