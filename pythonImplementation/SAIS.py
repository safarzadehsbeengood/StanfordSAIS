S_TYPE = ord("S")
L_TYPE = ord("L")

def buildTypeMap(data):
    res = bytearray(len(data) + 1) # makes a byte array to store types
    res[-1] = S_TYPE
    res[-2] = L_TYPE

    if not len(data):
        return res

    for i in range(len(data) - 2, -1, -1):
        if data[i] > data[i + 1]:
            res[i] = L_TYPE
        elif data[i] == data[i + 1] and res[i + 1] == L_TYPE:
            res[i] = L_TYPE
        else:
            res[i] = S_TYPE
    
    return res

def isLMSChar(offset, typemap):
    if offset == 0:
        return False
    if typemap[offset] == S_TYPE and typemap[offset-1] == L_TYPE:
        return True

    return False

def lmsSubstringsAreEqual(string, typemap, offsetA, offsetB):
    if offsetA == len(string) or offsetB == len(string):
        return False
    
    i = 0
    while True:
        aIsLMS = isLMSChar(i + offsetA, typemap)
        bIsLMS = isLMSChar(i + offsetB, typemap)

        if (i > 0 and aIsLMS and bIsLMS):
            return True
        if aIsLMS != bIsLMS:
            return False
        if string[i + offsetA] != string[i + offsetB]:
            return False
        i+=1

def findAlphabetSize(input_string):
    # Convert the input string to lowercase to handle both uppercase and lowercase letters
    input_string = input_string.lower()
    
    # Initialize a variable to keep track of the maximum letter's position
    max_position = -1
    
    for char in input_string:
        if 'a' <= char <= 'z':
            # Use ord() to get the ASCII value of the character and subtract the ASCII value of 'a'
            position = ord(char) - ord('a')
            if position > max_position:
                max_position = position

    return max_position + 1

def string_to_alphabet_position(input_string):
    # Convert the input string to lowercase to handle both uppercase and lowercase letters
    input_string = input_string.lower()
    
    # Initialize an empty list to store the positions
    positions = []
    
    # Iterate through the characters in the input string
    for char in input_string:
        if 'a' <= char <= 'z':
            # Use ord() to get the ASCII value of the character and subtract the ASCII value of 'a'
            position = ord(char) - ord('a')
            positions.append(position)
    
    return positions

def findBucketSizes(string, alphabetSize=256):
    encodedString = string_to_alphabet_position(string)
    res = [0] * alphabetSize
    print(alphabetSize)
    for char in encodedString:
        res[char] += 1
    return res

def findBucketHeads(bucketSizes):
    offset = 1
    res = []
    for size in bucketSizes:
        res.append(offset)
        offset += size
    return res

def findBucketTails(bucketSizes):
    offset = 1
    res = []

    for size in bucketSizes:
        offset += size
        res.append(offset - 1)
    
    return res
def showTypeMap(data):
    typemap = buildTypeMap(data)
    print(data.decode("ascii"))
    print(typemap.decode("ascii"))

    print("".join(
        "^" if isLMSChar(i, typemap) else " "
        for i in range(len(typemap))
    ))

def showSuffixArray(arr, pos = None):
    print(" ".join(
        "^^" if each == pos else "  " for each in range(len(arr))
    ))
    
def guessLMSSort(string, bucketSizes, typemap):
    guessedSuffixArray = [-1] * (len(string) + 1)
    bucketTails = findBucketTails(bucketSizes)
    for i in range(len(string)):
        if not isLMSChar(i, typemap):
            continue
        bucketIndex = string[i]
        guessedSuffixArray[bucketTails[bucketIndex]] = 1
        bucketTails[bucketIndex] -= 1

        showSuffixArray(guessedSuffixArray)
    guessedSuffixArray[0] = len(string)

    showSuffixArray(guessedSuffixArray)
    return guessedSuffixArray

def induceSortL(string, guessedSuffixArray, bucketSizes, typemap):
    bucketHeads = findBucketHeads(bucketSizes)

    for i in range(len(guessedSuffixArray)):
        if guessedSuffixArray[i] == -1:
            continue
        j = guessedSuffixArray[i] - 1
        if j < 0:
            continue
        if typemap[j] != L_TYPE:
            continue
        bucketIndex = string[j]
        guessedSuffixArray[bucketHeads[bucketIndex]] = j
        bucketHeads[bucketIndex] += 1

        showSuffixArray(guessedSuffixArray, i)

def induceSortS(string, guessedSuffixArray, bucketSizes, typemap):
    bucketTails = findBucketTails(bucketSizes)
    for i in range(len(guessedSuffixArray) - 1, -1, -1):
        j = guessedSuffixArray[i] - 1
        if j < 0:
            continue
        if typemap[j] != S_TYPE:
            continue
        bucketIndex = string[j]
        guessedSuffixArray[bucketTails[bucketIndex]] = j
        bucketTails[bucketIndex] -= 1
        showSuffixArray(guessedSuffixArray, i)

def summarizeSuffixArray(string, guessedSuffixArray, typemap):
    lmsNames = [-1] * (len(string) + 1)
    currentName = 0
    lastLMSSuffixOffset = None
    lmsNames[guessedSuffixArray[0]] = currentName
    lastLMSSuffixOffset = guessedSuffixArray[0]

    showSuffixArray(lmsNames)

    for i in range(1, len(guessedSuffixArray)):
        suffixOffset = guessedSuffixArray[i]

        if not isLMSChar(suffixOffset, typemap):
            continue
        if not lmsSubstringsAreEqual(string, typemap, lastLMSSuffixOffset, suffixOffset):
            currentName += 1

        lastLMSSuffixOffset = suffixOffset
        lmsNames[suffixOffset] = currentName
        showSuffixArray(lmsNames)
                        
    summarySuffixOffsets = []
    summaryString = []
    for index, name in enumerate(lmsNames):
        if name == -1:
            continue
        summarySuffixOffsets.append(index)
        summaryString.append(name)
    summaryAlphabetSize = currentName + 1
    return summaryString, summaryAlphabetSize, summarySuffixOffsets

def makeSummarySuffixArray(summaryString, summaryAlphabetSize):
    if summaryAlphabetSize == len(summaryString):
        summarySuffixArray = [-1] * (len(summaryString) + 1)
        summarySuffixArray[0] = len(summaryString)

        for x in range(len(summaryString)):
            y = summaryString[x]
            summarySuffixArray[y + 1] = x
    else:
        summarySuffixArray = SAIS(summaryString, summaryAlphabetSize)
    return summarySuffixArray

def accurateLMSSort(string, bucketSizes, typemap, summarySuffixArray, summarySuffixOffsets):
    suffixOffsets = [-1] * (len(string) + 1)
    bucketTails = findBucketTails(bucketSizes)
    for i in range(len(summarySuffixArray) - 1, 1, -1):
        stringIndex = summarySuffixOffsets[summarySuffixArray[i]]
        bucketIndex = string[stringIndex]
        suffixOffsets[bucketTails[bucketIndex]] = stringIndex
        bucketTails[bucketIndex] -= 1
        showSuffixArray(suffixOffsets)
    suffixOffsets[0] = len(string)
    showSuffixArray(suffixOffsets)
    return suffixOffsets

def SAIS(string):
    alphabetSize = findAlphabetSize(string)
    typemap = buildTypeMap(string)

    bucketSizes = findBucketSizes(string, alphabetSize)

    guessedSuffixArray = guessLMSSort(string, bucketSizes, typemap)

    induceSortL(string, guessedSuffixArray, bucketSizes, typemap)
    induceSortS(string, guessedSuffixArray, bucketSizes, typemap)

    summaryString, summaryAlphabetSize, summarySuffixOffsets = summarizeSuffixArray(string, guessedSuffixArray, typemap)

    summarySuffixArray = makeSummarySuffixArray(summaryString, summaryAlphabetSize)

    result = accurateLMSSort(string, bucketSizes, typemap, summarySuffixArray, summarySuffixOffsets)

    induceSortL(string, result, bucketSizes, typemap)
    induceSortS(string, result, bucketSizes, typemap)

    return result

SAIS("cabbage")