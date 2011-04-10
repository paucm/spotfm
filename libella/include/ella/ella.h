#ifndef ELLA_H
#define ELLA_H

#include <QByteArray>

namespace ella {

    class Ella {

        public:

            enum SimilarityType {
                Default,
                CollaborativeFiltering,
                Playlist
            };
            static QByteArray similarityTypeToString(SimilarityType type);
    };
};

#endif //ELLA_H
