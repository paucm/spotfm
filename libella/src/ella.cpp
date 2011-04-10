#include <ella/ella.h>

using namespace ella;

QByteArray Ella::similarityTypeToString(SimilarityType type)
{
     switch(type) {
        case CollaborativeFiltering:
            return "cf";
        case Playlist:
            return "playlist";
        default:
            return "";
    }
}
