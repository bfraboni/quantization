#ifndef KMEANS_H
#define KMEANS_H

#include <vector>
#include <random>
#include <limits>

template<class T>
class KMeans
{
public:
    int k, iter;
    std::vector<int> assignments, counts;
    std::vector<T> means, new_means;

    KMeans( const std::vector<T>& data, int k ) : 
        k(k), 
        assignments(data.size()),
        counts(this->k, 0),
        means(this->k, T()), 
        new_means(this->k, T())
    {
        static std::random_device seed;
        static std::mt19937 rng(seed());
        std::uniform_int_distribution<int> distribution(0, data.size() - 1);

        // Pick centroids as random points from the data
        for (auto& cluster : means) 
        {
            cluster = data[distribution(rng)];
        }

        int i = 0; 
        for (;; ++i) 
        {
            // Find assignments.
            // #pragma omp parallel for
            for (int id = 0; id < (int)data.size(); ++id) 
            {
                float best_distance = std::numeric_limits<float>::max();
                int best_cluster = 0;
                for (int cluster = 0; cluster < k; ++cluster) 
                {
                    const float distance = (data[id] - means[cluster]).length2();
                    if (distance < best_distance) 
                    {
                        best_distance = distance;
                        best_cluster = cluster;
                    }
                }
                assignments[id] = best_cluster;
                
                // Sum up and count points for each cluster.
                new_means[best_cluster] += data[id];
                ++counts[best_cluster];
            }

            // Divide sums by counts to get new centroids.
            float error = 0;
            for (int cluster = 0; cluster < k; ++cluster) 
            {
                // Turn 0/0 into 0/1 to avoid zero division.
                const auto count = std::max<int>(1, counts[cluster]);
                T new_mean = new_means[cluster] / std::max<int>(1, counts[cluster]);
                error += (means[cluster] - new_mean).length();
                means[cluster] = new_mean;
                new_means[cluster] = T();
                counts[cluster] = 0;
            }
            error /= k;

            if( error < 1e-4 ) break;
        }
        printf("iterations %d\n", i);
    }

    void replace(T * buffer)
    {
        #pragma omp parallel for
        for (int id = 0; id < (int)assignments.size(); ++id) 
        {
            buffer[id] = means[assignments[id]];
        }
    }
};

#endif