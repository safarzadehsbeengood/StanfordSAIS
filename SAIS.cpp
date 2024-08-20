#include "SAIS.h"
#include "DC3.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

size_t getUniqueChars(vector<size_t> text) {
  unordered_set<size_t> elems;
  size_t res = 0;
  for (size_t i = 0; i < text.size(); i++) {
    if (elems.find(text[i]) == elems.end()) {
      elems.insert(text[i]);
      res++;
    }
  }
  // cout << "uniqueChars: " << res << endl;
  return res;
}

bool hasDuplicates(vector<size_t> data) {
  unordered_set<size_t> elems;
  for (size_t element : data) {
    if (elems.count(element) > 0) {
      // cout << "has duplicates" << endl;
      return true;
    } else {
      elems.insert(element);
    }
  }
  // cout << "no duplicates" << endl;
  return false;
}

// function to print a vector
template <typename T> void printVector(const string &title, vector<T> &arr) {
  if (arr.empty()) {
    cout << title << ": []" << endl;
    return;
  }
  cout << title << ": [";
  for (size_t i = 0; i < arr.size() - 1; i++) {
    cout << arr[i] << ", ";
  }
  cout << arr[arr.size() - 1] << "]" << endl << endl;
}

// builds the typemap of a given string
vector<char> buildTypeMap(const vector<size_t> &text) {
  size_t n = text.size();
  vector<char> res(
      n, '$');      // result vector that will store the types of the suffixes
  res.back() = 'S'; // last char is always S type for null character

  if (text.empty()) { // return an array with one S-type for null character if
                      // string is empty
    return res;
  }

  res[res.size() - 2] =
      'L'; // if string is not empty, 2nd-to-last char is always L-type

  // step through rest of string from right to left and set type based on next
  // char
  for (int i = n - 2; i >= 0; i--) {
    if (text[i] >
        text[i + 1]) { // if current char is greater than next, it's an L-type
      res[i] = 'L';
    } else if (text[i] == text[i + 1] &&
               res[i + 1] == 'L') { // if current char is the same as the next
                                    // char, current char is L-type only if the
                                    // next char is an L-type
      res[i] = 'L';
    } else {
      res[i] = 'S';
    }
  }
  // cout << "typemap: [";
  // for (size_t i = 0; i < n - 1; i++) {
  //   cout << res[i] << " ";
  // }
  // cout << res[n - 1] << "]" << endl;

  return res;
}

// returns if a given index in the typemap is an LMS suffix
bool isLMSChar(int offset, const vector<char> &typemap) {
  if (offset == 0) {
    return false;
  }
  if (typemap[offset] == 'S' && typemap[offset - 1] == 'L') {
    return true;
  }
  return false;
}

/*
Returns a map, where the key is the index, and the value is which bucket it
belongs to.
*/
vector<size_t> findLMSSuffixes(const vector<size_t> &text,
                               vector<char> &typemap) {
  vector<size_t> res;
  for (size_t i = 0; i < typemap.size(); i++) {
    if (isLMSChar(i, typemap)) {
      res.push_back(i);
    }
  }
  // printVector("LMS Suffixes", res);
  return res;
}

vector<size_t> findBucketSizes(const vector<size_t> &text, size_t uniqueChars) {
  vector<size_t> res(uniqueChars); // stores occurrences of each char (ascii has
                                   // 128 characters)
  for (size_t i = 0; i < text.size(); i++) {
    res[text[i]]++;
  }
  // printVector("BUCKET SIZES", res);
  return res;
}

// returns a vector of beginning indexes for each char's bucket (ordered)
vector<size_t> findBucketHeads(const vector<size_t> &bucketSizes,
                               size_t uniqueChars) {
  size_t offset = 0;
  vector<size_t> res(uniqueChars);
  for (size_t i = 0; i < bucketSizes.size(); i++) {
    res[i] = offset;
    offset += bucketSizes[i];
  }
  // printVector("BUCKET HEADS", res);
  return res;
}

// find tail index of each bucket
vector<size_t> findBucketTails(const vector<size_t> &bucketSizes,
                               size_t uniqueChars) {
  size_t offset = 0;
  vector<size_t> res(uniqueChars);
  for (size_t i = 0; i < bucketSizes.size(); i++) {
    offset += bucketSizes[i];
    res[i] = offset - 1;
  }
  // printVector("BUCKET TAILS", res);
  return res;
}

vector<size_t> guessLMS(const vector<size_t> &text,
                        const vector<size_t> &bucketSizes,
                        const vector<size_t> &LMSSuffixes,
                        const vector<char> &typemap, size_t uniqueChars) {
  vector<size_t> bucketTails = findBucketTails(bucketSizes, uniqueChars);

  vector<size_t> SA(text.size()); // init SA
  SA[0] = text.size();            // sentinel

  // add LMS-types
  for (size_t i = 0; i < LMSSuffixes.size(); i++) {
    SA[bucketTails[text[LMSSuffixes[i]]]--] = LMSSuffixes[i];
  }
  // printVector("pass 1", SA);
  return SA;
}

void induceSortL(const vector<size_t> &text, vector<size_t> &SA,
                 const vector<size_t> &bucketSizes, const vector<char> &typemap,
                 size_t uniqueChars) {
  vector<size_t> bucketHeads = findBucketHeads(bucketSizes, uniqueChars);
  // add L-types
  for (size_t i = 0; i < text.size(); i++) {
    int j = SA[i] - 1;
    if (j < 0) {
      continue;
    }
    if (typemap[j] != 'L') {
      continue;
    }
    SA[bucketHeads[text[j]]++] = j;
  }
  // printVector("pass 2", SA);
}

void induceSortS(const vector<size_t> &text, vector<size_t> &SA,
                 const vector<size_t> &bucketSizes, const vector<char> &typemap,
                 size_t uniqueChars) {
  vector<size_t> bucketTails = findBucketTails(bucketSizes, uniqueChars);
  // add S-types
  for (int i = text.size() - 1; i >= 0; i--) {
    int j = SA[i] - 1;
    if (j < 0) {
      continue;
    }
    if (typemap[j] != 'S') {
      continue;
    }
    SA[bucketTails[text[j]]--] = j;
  }

  // printVector("pass 3", SA);
}

bool lmsSubstringsAreEqual(const vector<size_t> &text,
                           const vector<char> &typemap, size_t offsetA,
                           size_t offsetB) {
  size_t n = text.size();
  if (offsetA == n || offsetB == n) {
    return false;
  }

  int i = 0;
  while (true) {
    bool aIsLMS = isLMSChar(i + offsetA, typemap);
    bool bIsLMS = isLMSChar(i + offsetB, typemap);

    // cout << endl
    //      << "offsetA: " << offsetA << endl
    //      << "offsetB: " << offsetB << endl
    //      << "i: " << i << endl
    //      << "aIsLMS: " << aIsLMS << endl
    //      << "bIsLMS: " << bIsLMS << endl;
    // printVector("lmssae text", text);
    // printVector("lmssae typemap", typemap);

    if (i > 0 && aIsLMS && bIsLMS) {
      return true;
    }

    if (aIsLMS != bIsLMS) {
      return false;
    }

    if (text[i + offsetA] != text[i + offsetB]) {
      return false;
    }
    i++;
  }
  // return false;
}

struct summary {
  vector<size_t> string;
  vector<size_t> offsets;
};

summary reduceString(const vector<size_t> &text, const vector<size_t> &SA,
                     const vector<char> &typemap) {
  summary s;
  vector<int> LMSNames(text.size() + 1, -1); // convert to size_t vector later
  int currentName = 0;
  LMSNames[SA[0]] = currentName;
  int lastLMSOffset = SA[0];

  for (size_t i = 1; i < SA.size(); i++) {
    size_t offset = SA[i];
    if (!isLMSChar(offset, typemap)) {
      continue;
    }
    if (!lmsSubstringsAreEqual(text, typemap, lastLMSOffset, offset)) {
      currentName++;
    }
    lastLMSOffset = offset;
    LMSNames[offset] = currentName;
  }

  vector<size_t> str;
  vector<size_t> summaryOffsets;

  for (size_t i = 0; i < LMSNames.size(); i++) {
    if (LMSNames[i] == -1) {
      continue;
    }
    str.push_back(LMSNames[i]);
    summaryOffsets.push_back(i);
  }
  //
  // printVector("lms names", LMSNames);
  // printVector("summary offsets", summaryOffsets);
  // printVector("summary string", str);
  s.offsets = summaryOffsets;
  s.string = str;
  return s;
}

vector<size_t> makeSummarySA(summary summaryString) {
  if (hasDuplicates(summaryString.string)) {
    return sais(summaryString.string);
  } else {
    // cout << "mssa" << endl;
    // bucket sort
    vector<size_t> summarySA(summaryString.string.size() + 1);
    summarySA[0] = summaryString.string.size();
    for (size_t x = 0; x < summaryString.string.size(); x++) {
      int y = summaryString.string[x];
      summarySA[y + 1] = x;
    }
    // printVector("Summary SA", summarySA);
    return summarySA;
  }
}

vector<size_t> accurateLMSSort(const vector<size_t> &text,
                               const vector<size_t> &bucketSizes,
                               const vector<char> &typemap,
                               const vector<size_t> &summarySA,
                               const vector<size_t> &summarySuffixOffsets,
                               size_t uniqueChars) {
  vector<size_t> suffixOffsets(text.size());

  // printVector("Summary suffix offsets (ALS)", summarySuffixOffsets);

  vector<size_t> bucketTails = findBucketTails(bucketSizes, uniqueChars);
  for (int i = summarySA.size() - 1; i >= 1; i--) {
    int stringIndex = summarySuffixOffsets[summarySA[i]];
    int bucketIndex = text[stringIndex];

    suffixOffsets[bucketTails[bucketIndex]--] = stringIndex;
  }
  suffixOffsets[0] = text.size() - 1;
  // printVector("accurateLMSSort", suffixOffsets);
  return suffixOffsets;
}

SuffixArray sais(const vector<size_t> &text) {
  if (text.size() == 0) {
    vector<size_t> v = {0};
    return v;
  }

  size_t uniqueChars = getUniqueChars(text);
  vector<char> typemap = buildTypeMap(text);
  vector<size_t> LMSSuffixes = findLMSSuffixes(text, typemap);
  vector<size_t> bucketSizes = findBucketSizes(text, uniqueChars);
  vector<size_t> guessedSA =
      guessLMS(text, bucketSizes, LMSSuffixes, typemap, uniqueChars);
  induceSortL(text, guessedSA, bucketSizes, typemap, uniqueChars);
  induceSortS(text, guessedSA, bucketSizes, typemap, uniqueChars);
  summary reducedString = reduceString(text, guessedSA, typemap);
  vector<size_t> summarySA = makeSummarySA(reducedString);
  vector<size_t> result = accurateLMSSort(text, bucketSizes, typemap, summarySA,
                                          reducedString.offsets, uniqueChars);
  induceSortL(text, result, bucketSizes, typemap, uniqueChars);
  induceSortS(text, result, bucketSizes, typemap, uniqueChars);

  return result;
  // return dc3(text);
}
