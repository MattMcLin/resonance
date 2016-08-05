#include "stdafx.h"
#include "musictypes.h"
#include <Windows.h>
#include <random>
#include <string>
#include <stdlib.h>

/*

MAIN IDEA TO WORK ON NEXT
=========================
Instead of building the music up from beginning to end,
go through a series of permutations/iterations.
Start with just a measure or two, start subdividing in time and
substituting chords for more interesting ones.
The more iterations are applied, the more interesting things get.

Notes:
tonic
keys
inversions
modulation
scales
intervals
voicing
passing tones

phrase
motif
melody
harmony

instruments
phrasing


rhythym
spacing
measures


repeated ideas

structure


1. structure: A, AB, ABA, AABBA, ABCBA, ABCABCDAB
2. bars: 1,2,4,6,8
3. time sig: 2/4, 3/4, 4/4, 5/4, 6/4, 3/8, 6/8, 9/8, 12/8
4. starting key
*/

// TODO: define concept of tension that grows and releases over several chords/measures.
// When tension is released it should do so by biasing the weighting table below according
// to the desired resolution.

// TODO: nextDegreeWeight only accounts for degree of chord and not its type.
// But a dim7 chord of degree 0 better resolves to degree 4 than 0 in minor keys, or to degree 2 in major keys.
// Or the dim7 can be especially useful in modulations.
// Similarly, a tonic sus2 wants to resolve temporarily to the dominant.
float nextDegreeWeight[7][7] = {
    // C     D     E     F     G     A     B
    { 3.0f, 1.0f, 1.0f, 2.0f, 3.0f, 2.0f, 1.0f }, // C
    { 2.0f, 3.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }, // D
    { 2.0f, 3.0f, 3.0f, 3.0f, 1.0f, 2.0f, 1.0f }, // E
    { 3.0f, 1.0f, 1.0f, 3.0f, 3.0f, 1.0f, 1.0f }, // F
    { 5.0f, 1.0f, 1.0f, 2.0f, 3.0f, 2.0f, 1.0f }, // G
    { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 3.0f, 1.0f }, // A
    { 4.0f, 1.0f, 1.0f, 1.0f, 2.0f, 3.0f, 1.0f }, // B
};

float nextDegreeWeight_dim7[7][7] = {
    // C     D     E     F     G     A     B
    { 1.0f, 0.0f, 5.0f, 1.0f, 5.0f, 1.0f, 1.0f }, // C
    { 5.0f, 1.0f, 1.0f, 0.0f, 1.0f, 5.0f, 2.0f }, // D
    { 2.0f, 5.0f, 1.0f, 5.0f, 0.0f, 0.0f, 0.0f }, // E
    { 5.0f, 1.0f, 4.0f, 2.0f, 1.0f, 5.0f, 2.0f }, // F
    { 2.0f, 5.0f, 0.0f, 4.0f, 1.0f, 0.0f, 0.0f }, // G
    { 1.0f, 0.0f, 5.0f, 1.0f, 5.0f, 1.0f, 1.0f }, // A
    { 5.0f, 1.0f, 1.0f, 0.0f, 1.0f, 5.0f, 2.0f }, // B
};

int nextDegree(int degree, ChordType chordType)
{
    int newDegree = 0;

    float* pWeightTable = &(nextDegreeWeight[degree][0]);
    if (chordType == chord_dim7)
    {
        pWeightTable = &(nextDegreeWeight_dim7[degree][0]);
    }

    float totalWeight = 0.0f;
    for (int i = 0; i < 7; ++i)
    {
        totalWeight += pWeightTable[i];
    }

    float randomWeight = (static_cast<float>(rand() % 10000) / 10000.0f) * totalWeight;

    float weight = 0.0f;
    for (int i = 0; i < 7; ++i)
    {
        if ((randomWeight >= weight) && (randomWeight < (weight + pWeightTable[i])))
        {
            newDegree = i;
            break;
        }
        weight += pWeightTable[i];
    }

    return newDegree;
}

ChordType biggerChord(ChordType chord)
{
    switch (chord)
    {
    case chord_maj: return chord_maj7;
    case chord_min: return chord_min7;
    case chord_maj7: return chord_maj9;
    case chord_min7: return chord_min9;
    case chord_maj9: return chord_maj7b5;
    case chord_min9: return chord_min7b5;
    case chord_maj7b5: return chord_aug;
    case chord_aug: return chord_sus2;
    case chord_min7b5: return chord_sus2;
    case chord_sus2: return chord_sus4;
    case chord_sus4: return chord_dim;
    case chord_dim: return chord_dim7;
    case chord_dim7: return chord_dim;
    }
    return chord_maj;   // default case to make compiler happy
}

MusicChord nextChord(const MusicChord& chord, const Key& key)
{
    MusicChord newChord;
    newChord.degree = nextDegree(chord.degree, chord.chordType);

    if (key.isMajor)
    {
        newChord.chordType = keyChordsMajor[newChord.degree];
    }
    else
    {
        newChord.chordType = keyChordsMinor[newChord.degree];
    }
    int chordExpansion = rand() % (key.isMajor ? 9 : 8);
    while (chordExpansion-- > 0)
    {
        newChord.chordType = biggerChord(newChord.chordType);
    }

    newChord.inversion = rand() % 3;
    newChord.beats = static_cast<float>(rand() % 16) * 0.5f + 0.5f;

    return newChord;
}

bool chordIsMajor(ChordType chord)
{
    switch (chord)
    {
    case chord_maj:
    case chord_maj7:
    case chord_maj9:
        return true;
    default:
        return false;
    }
}

bool chordIsMinor(ChordType chord)
{
    switch (chord)
    {
    case chord_min:
    case chord_min7:
    case chord_min9:
        return true;
    default:
        return false;
    }
}

string beatsToString(float beats)
{
    string sBeats;
    int fullBeats = static_cast<int>(beats * 2.0f) / 2;
    int halfBeats = static_cast<int>(beats * 2.0f) % 2;
    for (; fullBeats > 0; fullBeats--)
    {
        sBeats.append("=");
    }
    if (halfBeats)
    {
        sBeats.append(".");
    }
    return sBeats;
}

string spellChord(const MusicChord& chord, const Key& key)
{
    string sChord;
    const int keyIndex = static_cast<int>(key.root);
    if (key.isMajor)
    {
        sChord = string(degreeNamesMajor[keyIndex][chord.degree]);
    }
    else
    {
        sChord = string(degreeNamesMinor[keyIndex][chord.degree]);
    }

    switch (chord.chordType)
    {
    case chord_min: sChord.append("-"); break;
    case chord_maj7: sChord.append("7"); break;
    case chord_min7: sChord.append("-7"); break;
    case chord_maj9: sChord.append("9"); break;
    case chord_min9: sChord.append("-9"); break;
    case chord_maj7b5: sChord.append("7b5"); break;
    case chord_min7b5: sChord.append("-7b5"); break;
    case chord_sus2: sChord.append("sus2"); break;
    case chord_sus4: sChord.append("sus4"); break;
    case chord_dim: sChord.append("dim"); break;
    case chord_dim7: sChord.append("dim7"); break;
    case chord_aug: sChord.append("+"); break;
    case chord_maj:
    default: 
        ; // do nothing
    }

    if (chord.inversion > 0)
    {
        // Only support inversions on simple major/minor chords.
        if (chordIsMajor(chord.chordType) || chordIsMinor(chord.chordType))
        {
            // Bass note for inversion is always within the key.
            // TODO: what about major dominant in minor key???
            if (key.isMajor)
            {
                sChord.append("/");
                int invDegree = (chord.degree + chord.inversion * 2) % 7;
                sChord.append(degreeNamesMajor[keyIndex][invDegree]);
            }
            else
            {
                sChord.append("/");
                int invDegree = (chord.degree + chord.inversion * 2) % 7;
                sChord.append(degreeNamesMinor[keyIndex][invDegree]);
            }
        }
    }

    sChord.append(beatsToString(chord.beats));

    return sChord;
}

void printChords(const vector<MusicChord>& chords, const Key& key, string& sBuffer)
{
    sBuffer.append("Key of ");
    sBuffer.append(degreeNamesMajor[key.root][0]);
    if (key.isMajor)
    {
        sBuffer.append(" major:\n");
    }
    else
    {
        sBuffer.append(" minor:\n");
    }
    
    for (size_t i = 0; i < chords.size(); ++i)
    {
        string sChord = spellChord(chords[i], key);
        sBuffer.append(sChord);
        sBuffer.append(" ");
    }
}

string writeSection()
{
    Key key;
    key.root = static_cast<KeyRoot>(rand() % NumberOfKeys);
    key.isMajor = ((rand() % 2) == 0);

    vector<MusicChord> chords;
    vector<MusicChord> chordMotif;
    
    // start with the tonic
    MusicChord tonic;
    tonic.chordType = key.isMajor ? chord_maj : chord_min;
    tonic.inversion = 0;
    tonic.degree = 0;
    tonic.beats = static_cast<float>(rand() % 16) * 0.5f + 0.5f;
    chordMotif.push_back(tonic);
    
    int beatChoices[] = { 4, 8, 12, 16, 24, 32 };
    int totalBeatsInMotif = beatChoices[rand() % (sizeof(beatChoices) / sizeof(int))];
    float motifBeats = static_cast<float>(totalBeatsInMotif) - tonic.beats;

    MusicChord chord = tonic;
    while (motifBeats > 0.0f)
    {
        // todo: subtract motifBeats
        chord = nextChord(chord, key);

        // If this chord duration is longer than what we have
        // left in our measure, then clamp its duration.
        if (chord.beats > motifBeats)
        {
            chord.beats = motifBeats;
        }
        motifBeats -= chord.beats;
        chordMotif.push_back(chord);
    }

    int numRepeats = (rand() % 4) + 1;
    for (; numRepeats > 0; --numRepeats)
    {
        chords.insert(chords.end(), chordMotif.begin(), chordMotif.end());
    }   

    // Always end with the tonic.
    chords.push_back(tonic);

    string sBuffer;
    printChords(chords, key, sBuffer);
    sBuffer.append("\n\nMotif beats = ");
    sBuffer.append(to_string(totalBeatsInMotif));
    return sBuffer;
}