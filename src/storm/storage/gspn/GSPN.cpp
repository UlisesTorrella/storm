#include "storm/storage/gspn/GSPN.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include <unordered_map>

#include <boost/lexical_cast.hpp>

namespace storm {
    namespace gspn {
        uint64_t GSPN::timedTransitionIdToTransitionId(uint64_t ttId) {
            return ttId | (1ull << ((sizeof(ttId) * CHAR_BIT) - 1));
        }
        
        uint64_t GSPN::immediateTransitionIdToTransitionId(uint64_t itId) {
            return itId;
        }
        
        uint64_t GSPN::transitionIdToTimedTransitionId(uint64_t tId) {
            return (tId << 1) >> 1;
        }
        
        uint64_t GSPN::transitionIdToImmediateTransitionId(uint64_t tId) {
            return tId;
        }
        
        GSPN::GSPN(std::string const& name, std::vector<Place> const& places, std::vector<ImmediateTransition<WeightType>> const& itransitions, std::vector<TimedTransition<RateType>> const& ttransitions)
        : name(name), places(places), immediateTransitions(itransitions), timedTransitions(ttransitions)
        {
            
        }
        
        uint64_t GSPN::getNumberOfPlaces() const {
            return places.size();
        }

        std::vector<storm::gspn::TimedTransition<GSPN::RateType>> const& GSPN::getTimedTransitions() const {
            return this->timedTransitions;
        }

        std::vector<storm::gspn::ImmediateTransition<GSPN::WeightType>> const& GSPN::getImmediateTransitions() const {
            return this->immediateTransitions;
        }

        std::vector<storm::gspn::Place> const& GSPN::getPlaces() const {
            return places;
        }

        std::shared_ptr<storm::gspn::Marking> GSPN::getInitialMarking(std::map<uint64_t, uint64_t>& numberOfBits, uint64_t const& numberOfTotalBits) const {
            auto m = std::make_shared<storm::gspn::Marking>(getNumberOfPlaces(), numberOfBits, numberOfTotalBits);
            for (auto& place : getPlaces()) {
                m->setNumberOfTokensAt(place.getID(), place.getNumberOfInitialTokens());
            }
            return m;
        }

        
        storm::gspn::Place const* GSPN::getPlace(uint64_t id) const {
            if(id < places.size()) {
                assert(places.at(id).getID() == id);
                return &places.at(id);
            }
            return nullptr;
        }
        
        
        storm::gspn::Place* GSPN::getPlace(uint64_t id) {
            if(id < places.size()) {
                assert(places.at(id).getID() == id);
                return &places.at(id);
            }
            return nullptr;
        }
        
      
        storm::gspn::Place const* GSPN::getPlace(std::string const& name) const {
            for (auto& place : places) {
                if (place.getName() == name) {
                    return &place;
                }
            }
            return nullptr;
        };
        
        storm::gspn::Place* GSPN::getPlace(std::string const& name) {
            for (auto& place : places) {
                if (place.getName() == name) {
                    return &place;
                }
            }
            return nullptr;
        };

        storm::gspn::TimedTransition<GSPN::RateType> const* GSPN::getTimedTransition(std::string const& name) const {
            for (auto& trans : timedTransitions) {
                if (name == trans.getName()) {
                    return &trans;
                }
            }
            return nullptr;
        }

        storm::gspn::ImmediateTransition<GSPN::WeightType> const* GSPN::getImmediateTransition(std::string const& name) const {
            for (auto& trans : immediateTransitions) {
                if (name == trans.getName()) {
                    return &trans;
                }
            }
            return nullptr;
        }
        
        

        storm::gspn::Transition const* GSPN::getTransition(std::string const& id) const {
            auto trans = getTimedTransition(id);
            if (trans != nullptr) {
                return trans;
            }

            return getImmediateTransition(id);
        }
        
        void GSPN::setCapacities(std::unordered_map<std::string, uint64_t> const& mapping) {
            for(auto const& entry : mapping) {
                storm::gspn::Place* place = getPlace(entry.first);
                STORM_LOG_THROW(place != nullptr, storm::exceptions::InvalidArgumentException, "No place with name " << entry.first);
                place->setCapacity(entry.second);
                
            }
        }

        void GSPN::writeDotToStream(std::ostream& outStream) const {
            outStream << "digraph " << this->getName() << " {" << std::endl;

            // print places with initial marking (not printed is the capacity)
            outStream << "\t" << "node [shape=ellipse]" << std::endl;
            for (auto& place : this->getPlaces()) {
                outStream << "\t" << place.getName() << " [label=\"" << place.getName() << "(" << place.getNumberOfInitialTokens();
                outStream << ")";
                if(place.hasRestrictedCapacity()) {
                    outStream << "c " << place.getCapacity();
                }
                
                outStream << "\"];" << std::endl;
            }

            // print transitions with weight/rate
            outStream << "\t" << "node [shape=box]" << std::endl;
            for (auto& trans : this->getImmediateTransitions()) {
                outStream << "\t" << trans.getName() << " [fontcolor=white, style=filled, fillcolor=black, label=\"" << trans.getName() << "\"];" << std::endl;
            }

            for (auto& trans : this->getTimedTransitions()) {
                outStream << "\t" << trans.getName() << " [label=\"" << trans.getName();
                outStream << " (" << trans.getRate() << ")\"];" << std::endl;
            }

            // print arcs
            for (auto& trans : this->getImmediateTransitions()) {

                for (auto const& inEntry : trans.getInputPlaces()) {
                    outStream << "\t" << places.at(inEntry.first).getName() << " -> " << trans.getName() << "[label=\"" << inEntry.second << "\"];" << std::endl;
                }

                for (auto const& inhEntry : trans.getInhibitionPlaces()) {
                    outStream << "\t" << places.at(inhEntry.first).getName() << " -> " << trans.getName() << "[arrowhead=\"dot\", label=\"" << inhEntry.second << "\"];" << std::endl;
                }

                for (auto const& outEntry : trans.getOutputPlaces()) {
                    outStream << "\t" << trans.getName() << " -> " << places.at(outEntry.first).getName() << "[label=\"" << outEntry.second << "\"];" << std::endl;
                }
            }

            for (auto& trans : this->getTimedTransitions()) {
                for (auto const& inEntry : trans.getInputPlaces()) {
                    outStream << "\t" << places.at(inEntry.first).getName() << " -> " << trans.getName() << "[label=\"" << inEntry.second << "\"];" << std::endl;
                }
                
                for (auto const& inhEntry : trans.getInhibitionPlaces()) {
                    outStream << "\t" << places.at(inhEntry.first).getName() << " -> " << trans.getName() << "[arrowhead=\"dot\", label=\"" << inhEntry.second << "\"];" << std::endl;
                }
                
                for (auto const& outEntry : trans.getOutputPlaces()) {
                    outStream << "\t" << trans.getName() << " -> " << places.at(outEntry.first).getName() << "[label=\"" << outEntry.second << "\"];" << std::endl;
                }
            }

            outStream << "}" << std::endl;
        }
        
        void GSPN::setName(std::string const& name) {
            this->name = name;
        }

        std::string const& GSPN::getName() const {
            return this->name;
        }

        bool GSPN::isValid() const {
            bool result = true;
            result |= testPlaces();
            result |= testTransitions();

            return result;
        }

        bool GSPN::testPlaces() const {
            std::vector<std::string> namesOfPlaces;
            std::vector<uint64_t> idsOfPlaces;
            bool result = true;

            for (auto const& place : this->getPlaces()) {

                if (std::find(namesOfPlaces.begin(), namesOfPlaces.end(), place.getName()) != namesOfPlaces.end()) {
                    STORM_PRINT_AND_LOG("duplicates states with the name \"" + place.getName() + "\"\n")
                    result = false;
                }

                if (std::find(idsOfPlaces.begin(), idsOfPlaces.end(), place.getID()) != idsOfPlaces.end()) {
                    STORM_PRINT_AND_LOG("duplicates states with the id \"" + boost::lexical_cast<std::string>(place.getID()) + "\"\n")
                    result = false;
                }

                if (place.getNumberOfInitialTokens() > place.getNumberOfInitialTokens()) {
                    STORM_PRINT_AND_LOG("number of initial tokens is greater than the capacity for place \"" + place.getName() + "\"\n")
                    result = false;
                }
            }

            return result;
        }

        bool GSPN::testTransitions() const {
            bool result = true;

//            for (auto const& transition : this->getImmediateTransitions()) {
//                if (transition.getInputPlaces().empty() &&
//                    transition.getInhibitionPlaces().empty()) {
//                    STORM_PRINT_AND_LOG("transition \"" + transition.getName() + "\" has no input or inhibition place\n")
//                    result = false;
//                }
//
//                if (transition.getOutputPlaces().empty()) {
//                    STORM_PRINT_AND_LOG("transition \"" + transition.getName() + "\" has no output place\n")
//                    result = false;
//                }
//            }
//
//            for (auto const& transition : this->getTimedTransitions()) {
//                if (transition.getInputPlaces().empty() &&
//                    transition.getInputPlaces().empty()) {
//                    STORM_PRINT_AND_LOG("transition \"" + transition.getName() + "\" has no input or inhibition place\n")
//                    result = false;
//                }
//
//                if (transition.getOutputPlaces().empty()) {
//                    STORM_PRINT_AND_LOG("transition \"" + transition.getName() + "\" has no output place\n")
//                    result = false;
//                }
//            }
//
//            //test if places exists in the gspn
//            for (auto const& transition : this->getImmediateTransitions()) {
//                for (auto &placePtr : transition.getInputPlaces()) {
//                    bool foundPlace = false;
//                    for (auto const& place : places) {
//                        if (place.getName() == placePtr->getName()) {
//                            foundPlace = true;
//                        }
//                    }
//                    if (!foundPlace) {
//                        STORM_PRINT_AND_LOG("input place \"" + placePtr->getName() + "\" of transition \"" + transition.getName() + "\" was not found \n")
//                        result = false;
//                    }
//                }
//
//                for (auto &placePtr : transition.getInhibitionPlaces()) {
//                    bool foundPlace = false;
//                    for (auto const& place : places) {
//                        if (place.getName() == placePtr->getName()) {
//                            foundPlace = true;
//                        }
//                    }
//                    if (!foundPlace) {
//                        STORM_PRINT_AND_LOG("inhibition place \"" + placePtr->getName() + "\" of transition \"" + transition.getName() + "\" was not found \n")
//                        result = false;
//                    }
//                }
//
//                for (auto &placePtr : transition.getOutputPlaces()) {
//                    bool foundPlace = false;
//                    for (auto const& place : places) {
//                        if (place.getName() == placePtr->getName()) {
//                            foundPlace = true;
//                        }
//                    }
//                    if (!foundPlace) {
//                        STORM_PRINT_AND_LOG("output place \"" + placePtr->getName() + "\" of transition \"" + transition.getName() + "\" was not found \n")
//                        result = false;
//                    }
//                }
//            }
//
//            for (auto const& transition : this->getTimedTransitions()) {
//                for (auto &placePtr : transition.getInputPlaces()) {
//                    bool foundPlace = false;
//                    for (auto const& place : places) {
//                        if (place.getName() == placePtr->getName()) {
//                            foundPlace = true;
//                        }
//                    }
//                    if (!foundPlace) {
//                        STORM_PRINT_AND_LOG("input place \"" + placePtr->getName() + "\" of transition \"" + transition.getName() + "\" was not found \n")
//                        result = false;
//                    }
//                }
//
//                for (auto &placePtr : transition.getInhibitionPlaces()) {
//                    bool foundPlace = false;
//                    for (auto const& place : places) {
//                        if (place.getName() == placePtr->getName()) {
//                            foundPlace = true;
//                        }
//                    }
//                    if (!foundPlace) {
//                        STORM_PRINT_AND_LOG("inhibition place \"" + placePtr->getName() + "\" of transition \"" + transition.getName() + "\" was not found \n")
//                        result = false;
//                    }
//                }
//
//                for (auto &placePtr : transition.getOutputPlaces()) {
//                    bool foundPlace = false;
//                    for (auto const& place : places) {
//                        if (place.getName() == placePtr->getName()) {
//                            foundPlace = true;
//                        }
//                    }
//                    if (!foundPlace) {
//                        STORM_PRINT_AND_LOG("output place \"" + placePtr->getName() + "\" of transition \"" + transition.getName() + "\" was not found \n")
//                        result = false;
//                    }
//                }
//            }


            return result;
        }

        void GSPN::toPnpro(std::ostream &stream) const {
            auto space = "  ";
            auto space2 = "    ";
            auto space3 = "      ";
            auto projectName = "storm-export"; // TODO add to args
            stream << "<project name=\"" << projectName << "\" version=\"121\">" << std::endl;
            stream << space << "<gspn name=\"" << getName() << "\" >" << std::endl;

            u_int32_t x = 1;
            stream << space2 << "<nodes>" << std::endl;
            for (auto& place : places) {
                stream << space3 << "<place marking=\"" << place.getNumberOfInitialTokens() <<"\" ";
                stream << "name =\"" << place.getName() << "\" ";
                stream << "x=\"" << x << "\" ";
                stream << "y=\"1\" ";
                stream << "/>" << std::endl;
                x = x + 3;
            }
            x = 1;
            for (auto& trans : timedTransitions) {
                stream << space3 << "<transition name=\"" << trans.getName() << "\" ";
                stream << "type=\"EXP\" ";
                stream << "nservers-x=\"" << trans.getRate() << "\" ";
                stream << "x=\"" << x << "\" ";
                stream << "y=\"4\" ";
                stream << "/>" << std::endl;
                x = x + 3;
            }
            for (auto& trans : immediateTransitions) {
                stream << space3 << "<transition name=\"" << trans.getName() << "\" ";
                stream << "type=\"IMM\" ";
                stream << "x=\"" << x << "\" ";
                stream << "y=\"4\" ";
                stream << "/>" << std::endl;
                x = x + 3;
            }
            stream << space2 << "</nodes>" << std::endl;

            stream << space2 << "<edges>" << std::endl;
            for (auto& trans : timedTransitions) {
                for (auto const& inEntry : trans.getInputPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << trans.getName() << "\" ";
                    stream << "tail=\"" << places.at(inEntry.first).getName() << "\" ";
                    stream << "kind=\"INPUT\" ";
                    stream << "mult=\"" << inEntry.second << "\" ";
                    stream << "/>" << std::endl;
                }
                for (auto const& inhEntry : trans.getInhibitionPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << trans.getName() << "\" ";
                    stream << "tail=\"" << places.at(inhEntry.first).getName() << "\" ";
                    stream << "kind=\"INHIBITOR\" ";
                    stream << "mult=\"" << inhEntry.second << "\" ";
                    stream << "/>" << std::endl;
                }
                for (auto const& outEntry : trans.getOutputPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << places.at(outEntry.first).getName() << "\" ";
                    stream << "tail=\"" << trans.getName() << "\" ";
                    stream << "kind=\"OUTPUT\" ";
                    stream << "mult=\"" << outEntry.second << "\" ";
                    stream << "/>" << std::endl;
                }
            }
            for (auto& trans : immediateTransitions) {
                for (auto const& inEntry : trans.getInputPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << trans.getName() << "\" ";
                    stream << "tail=\"" << places.at(inEntry.first).getName() << "\" ";
                    stream << "kind=\"INPUT\" ";
                    stream << "mult=\"" << inEntry.second << "\" ";
                    stream << "/>" << std::endl;
                }
                for (auto const& inhEntry : trans.getInhibitionPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << trans.getName() << "\" ";
                    stream << "tail=\"" << places.at(inhEntry.first).getName() << "\" ";
                    stream << "kind=\"INHIBITOR\" ";
                    stream << "mult=\"" << inhEntry.second << "\" ";
                    stream << "/>" << std::endl;
                }
                for (auto const& outEntry : trans.getOutputPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << places.at(outEntry.first).getName() << "\" ";
                    stream << "tail=\"" << trans.getName() << "\" ";
                    stream << "kind=\"OUTPUT\" ";
                    stream << "mult=\"" << outEntry.second << "\" ";
                    stream << "/>" << std::endl;
                }
            }
            stream << space2 << "</edges>" << std::endl;
            stream << space << "</gspn>" << std::endl;
            stream << "</project>" << std::endl;
        }

        void GSPN::toPnml(std::ostream &stream) const {
            std::string space = "  ";
            std::string space2 = "    ";
            std::string space3 = "      ";
            std::string space4 = "        ";

            stream << "<pnml>" << std::endl;
            stream << space << "<net id=\"" << getName() << "\">" << std::endl;

            // add places
            for (const auto &place : places) {
                stream << space2 << "<place id=\"" << place.getName() << "\">" << std::endl;
                stream << space3 << "<initialMarking>" << std::endl;
                stream << space4 << "<value>Default," << place.getNumberOfInitialTokens() << "</value>" << std::endl;
                stream << space3 << "</initialMarking>" << std::endl;
                stream << space2 << "</place>" << std::endl;
            }

            // add immediate transitions
            for (const auto &trans : immediateTransitions) {
                stream << space2 << "<transition id=\"" << trans.getName() << "\">" << std::endl;
                stream << space3 << "<rate>" << std::endl;
                stream << space4 << "<value>" << trans.getWeight() << "</value>" << std::endl;
                stream << space3 << "</rate>" << std::endl;
                stream << space3 << "<timed>" << std::endl;
                stream << space4 << "<value>false</value>" << std::endl;
                stream << space3 << "</timed>" << std::endl;
                stream << space2 << "</transition>" << std::endl;
            }

            // add timed transitions
            for (const auto &trans : timedTransitions) {
                stream << space2 << "<transition id=\"" << trans.getName() << "\">" << std::endl;
                stream << space3 << "<rate>" << std::endl;
                stream << space4 << "<value>" << trans.getRate() << "</value>" << std::endl;
                stream << space3 << "</rate>" << std::endl;
                stream << space3 << "<timed>" << std::endl;
                stream << space4 << "<value>true</value>" << std::endl;
                stream << space3 << "</timed>" << std::endl;
                stream << space2 << "</transition>" << std::endl;
            }

            uint64_t i = 0;
            // add arcs for immediate transitions
            for (const auto &trans : immediateTransitions) {
                // add input arcs
                for (auto const& inEntry : trans.getInputPlaces()) {
                    stream << space2 << "<arc ";
                    stream << "id=\"arc" << i++ << "\" ";
                    stream << "source=\"" << places.at(inEntry.first).getName() << "\" ";
                    stream << "target=\"" << trans.getName() << "\" ";
                    stream << ">" << std::endl;

                    stream << space3 << "<inscription>" << std::endl;
                    stream << space4 << "<value>Default," << inEntry.second << "</value>" << std::endl;
                    stream << space3 << "</inscription>" << std::endl;

                    stream << space3 << "<type value=\"normal\" />" << std::endl;

                    stream << space2 << "</arc>" << std::endl;
                }

                // add inhibition arcs
                for (auto const& inhEntry : trans.getInhibitionPlaces()) {
                    stream << space2 << "<arc ";
                    stream << "id=\"arc" << i++ << "\" ";
                    stream << "source=\"" << places.at(inhEntry.first).getName() << "\" ";
                    stream << "target=\"" << trans.getName() << "\" ";
                    stream << ">" << std::endl;

                    stream << space3 << "<inscription>" << std::endl;
                    stream << space4 << "<value>Default," << inhEntry.second << "</value>" << std::endl;
                    stream << space3 << "</inscription>" << std::endl;

                    stream << space3 << "<type value=\"inhibition\" />" << std::endl;

                    stream << space2 << "</arc>" << std::endl;
                }

                // add output arcs
                for (auto const& outEntry : trans.getOutputPlaces()) {
                    stream << space2 << "<arc ";
                    stream << "id=\"arc" << i++ << "\" ";
                    stream << "source=\"" << trans.getName() << "\" ";
                    stream << "target=\"" << places.at(outEntry.first).getName() << "\" ";
                    stream << ">" << std::endl;

                    stream << space3 << "<inscription>" << std::endl;
                    stream << space4 << "<value>Default," << outEntry.second << "</value>" << std::endl;
                    stream << space3 << "</inscription>" << std::endl;

                    stream << space3 << "<type value=\"normal\" />" << std::endl;

                    stream << space2 << "</arc>" << std::endl;
                }
            }

            stream << space << "</net>" << std::endl;
            stream << "</pnml>" << std::endl;
        }
    }
}


