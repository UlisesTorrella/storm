#ifndef STORM_STORAGE_GEOMETRY_HALFSPACE_H_
#define STORM_STORAGE_GEOMETRY_HALFSPACE_H_

#include <iostream>
#include <iomanip>
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"

namespace storm {
    namespace storage {
        namespace geometry {
            
            /* 
             * This class represents a closed Halfspace, i.e., the set { x | a*x<=c } for a normalVector a and an offset c
             */
             
            template <typename ValueType>
            class Halfspace {
                
            public:
                
                Halfspace(std::vector<ValueType> const& normalVector, ValueType const& offset) : mNormalVector(normalVector), mOffset(offset)  {
                    //Intentionally left empty
                }
                
                Halfspace(std::vector<ValueType>&& normalVector, ValueType&& offset) : mNormalVector(normalVector), mOffset(offset) {
                    //Intentionally left empty
                }
                
                /*
                 * Returns true iff the given point is contained in this halfspace, i.e., normalVector*point <= offset holds.
                 */
                bool contains(std::vector<ValueType> const& point) const {
                    return storm::utility::vector::dotProduct(point, normalVector()) <= offset();
                }
                
                /*
                 * Returns the (scaled) distance of the given point from this halfspace.
                 * If the point is inside this halfspace, the distance is 0.
                 * The returned value is the euclidean distance times the 2-norm of the normalVector.
                 * In contrast to the euclideanDistance method, there are no inaccuracies introduced (providing ValueType is exact for +, -, and *)
                 */
                ValueType distance(std::vector<ValueType> const& point) const {
                    return std::max(storm::utility::zero<ValueType>(), storm::utility::vector::dotProduct(point, normalVector()) - offset());
                }
                
                /*
                 * Returns the euclidean distance of the point from this halfspace.
                 * If the point is inside this halfspace, the distance is 0.
                 * Note that the euclidean distance is in general not a rational number (which can introduce inaccuracies).
                 */
                ValueType euclideanDistance(std::vector<ValueType> const& point) const {
                    // divide the distance with the 2-norm of the normal vector
                    return distance(point) / storm::utility::sqrt(storm::utility::vector::dotProduct(normalVector(), normalVector()));
                }
                
                /*
                 * Returns true iff the given point lies on the boundary of this halfspace (i.e., on the hyperplane given by normalVector()*x =offset
                 */
                bool isPointOnBoundary(std::vector<ValueType> const& point) const {
                    return storm::utility::vector::dotProduct(point, normalVector()) == offset();
                }
                
                /*
                 * Returns the inverted Halfspace of this which represents the set (R^n \ this) union { x | x is on the boundary of this}
                 */
                Halfspace<ValueType> invert() const {
                    std::vector<ValueType> resNormalVector = normalVector();
                    storm::utility::vector::scaleVectorInPlace(resNormalVector, -storm::utility::one<ValueType>());
                    return Halfspace<ValueType>(std::move(resNormalVector), -offset());
                }

                /*
                 * Returns a string representation of this Halfspace.
                 * If the given flag is true, the occurring numbers are converted to double before printing to increase readability
                 */
                std::string toString(bool numbersAsDouble = false) const {
                    std::stringstream stream;
                    stream << "(";
                    for(auto it = normalVector().begin(); it != normalVector().end(); ++it){
                        if(it != normalVector().begin()){
                            stream << ", ";
                        }
                        std::stringstream numberStream;
                        if(numbersAsDouble) {
                            numberStream << storm::utility::convertNumber<double>(*it);
                        } else {
                            numberStream << *it;
                        }
                        stream << std::setw(10) << numberStream.str();
                    }
                    stream << ") * x <= ";
                    if(numbersAsDouble) {
                        stream << storm::utility::convertNumber<double>(offset());
                    } else {
                        stream << offset();
                    }
                    return stream.str();
                }
                
                std::vector<ValueType> const& normalVector() const {
                    return mNormalVector;
                }
                
                std::vector<ValueType>& normalVector(){
                    return mNormalVector;
                }
                
                ValueType const& offset() const {
                    return mOffset;
                }
                
                ValueType& offset(){
                    return mOffset;
                }
                
            private:
                
                std::vector<ValueType> mNormalVector;
                ValueType mOffset;
                
            };
        }
    }
}

#endif /* STORM_STORAGE_GEOMETRY_HALFSPACE_H_ */
