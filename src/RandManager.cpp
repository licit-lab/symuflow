#include "stdafx.h"
#include "RandManager.h"

// valeur max de la distribution � MAXIMUM_RANDOM_NUMBER-1 pour �viter les probl�mes de test strict si on tire la valeur max comme c'est le cas parfois...
RandManager::RandManager() : m_Count(0), m_RandomGenerator(), m_Distribution(0, MAXIMUM_RANDOM_NUMBER - 1), m_LimitedInt(m_RandomGenerator, m_Distribution)
{
}

/// Initialisation de la s�quence al�atoire
void RandManager::mySrand(unsigned int seed, unsigned int nbCalls)
{
    m_Count = nbCalls;

    // initialisation de la s�quence
    //srand(seed);
    m_RandomGenerator.seed(seed);

    // retour � l'�l�ment en cours
    for (unsigned int i = 0; i < nbCalls; i++)
    {
        // rmq. : on suppose que les tirages li�s � la loi de poisson ou la loi binomiale incr�mentent autant le g�n�rateur al�atoire que les tirage classiques ... (� v�rifier ?)
        m_LimitedInt();
    }
}

int RandManager::myRand()
{
    m_Count++;
    return m_LimitedInt();
}

int RandManager::myPoissonRand(double mean)
{
    m_Count++;

    boost::poisson_distribution<> poisson(mean);
    boost::variate_generator< boost::mt19937&, boost::poisson_distribution<> > poissonGenerator(m_RandomGenerator, poisson);
    return poissonGenerator();
}

int RandManager::myBinomialRand(double probability, int trialNb)
{
    m_Count++;

    boost::binomial_distribution<> binomial(trialNb, probability);
    boost::variate_generator< boost::mt19937&, boost::binomial_distribution<> > binomialGenerator(m_RandomGenerator, binomial);
    return binomialGenerator();
}

double RandManager::myExponentialRand(double rate)
{
    m_Count++;

    boost::exponential_distribution<> exponential(rate);
    boost::variate_generator< boost::mt19937&, boost::exponential_distribution<> > exponentialGenerator(m_RandomGenerator, exponential);
    return exponentialGenerator();
}

double RandManager::myNormalRand(double dbMean, double dbSigma)
{
    m_Count++;

    boost::normal_distribution<> normal(dbMean, dbSigma);
    boost::variate_generator< boost::mt19937&, boost::normal_distribution<> > normalGenerator(m_RandomGenerator, normal);
    return normalGenerator();
}

double RandManager::myNormalRand(double dbMean, double dbSigma, double dbMin, double dbMax)
{
    double  dbVal;

    while (1)
    {
        dbVal = myNormalRand(dbMean, dbSigma);
        if (dbVal >= dbMin && dbVal <= dbMax)
            return dbVal;
    }

    return -1;
}

unsigned int RandManager::getCount()
{
    return m_Count;
}