//
//  myrandom.h
//  RandomWriter
//
//  Created by Shanon Reckinger.
//  Copyright © 2020 Shanon Reckinger. All rights reserved.
//

#pragma once

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>

using namespace std;

bool useAutograder = false;
int seed = 15;
/*
 * Function: randomInteger
 * Usage: int n = randomInteger(low, high);
 * ----------------------------------------
 * Returns a random integer in the range low to
 * high, inclusive.
 */
int randomInteger(int low, int high) {
    static bool initialized = false;
    if (!initialized) {
        if (useAutograder) {
            srand(seed);
            rand();
        }
        else {
            srand(int(time(NULL)));
            rand();   // throwaway call to get randomness going
        }
        initialized = true;
    }
    double d = rand() / (double(RAND_MAX) + 1);
    double s = d * (double(high) - low + 1);
    return int(floor(low + s));
}

// driver.h
// Personality Tests Project
// Chua cac ham cot loi xu ly quiz OCEAN:
//   randomQuestionFrom, scoresFrom, normalize,
//   cosineSimilarityOf, mostSimilarTo

#include <math.h>
#include <limits>
#include <string>
#include <map>
#include <set>
#include "myrandom.h"

using namespace std;

constexpr double lowest_double = std::numeric_limits<double>::lowest();

/* Type: Question
 *
 * Type representing a personality quiz question.
 */
struct Question {
    string questionText;  // Text of the question
    map<char, int> factors;   // Map from factors to +1 or -1
    friend bool operator< (const Question& lhs, const Question& rhs) {
        return lhs.questionText < rhs.questionText;
    }
    friend bool operator== (const Question& lhs, const Question& rhs) {
        return lhs.questionText == rhs.questionText;
    }
    friend bool operator!= (const Question& lhs, const Question& rhs) {
        return lhs.questionText != rhs.questionText;
    }
};

/* Type: Person
 *
 * Type representing a person, used to represent people when determining
 * who's the closest match to the user.
 */
struct Person {
    string name;      // Name of the person
    map<char, int> scores;  // Map from factors to +1 or -1
    friend bool operator< (const Person& lhs, const Person& rhs) {
        return lhs.name < rhs.name;
    }
    friend bool operator== (const Person& lhs, const Person& rhs) {
        return lhs.name == rhs.name;
    }
    friend bool operator!= (const Person& lhs, const Person& rhs) {
        return lhs.name != rhs.name;
    }
};

/* randomElement
 *
 * This function selects, at random, a Question from the inputted questions set
 * and returns the question.  Note, this function does not remove the randomly
 * selected question from the set.
*/
Question randomElement(set<Question>& questions) {
    int ind = randomInteger(0, (int)questions.size() - 1);
    int i = 0;
    for (auto e : questions) {
        if (i == ind) {
            return e;
        }
        i++
            ;
    }
    return {};
}

/* randomQuestionFrom
 *
 * Chon ngau nhien mot Question tu tap questions,
 * XOA no khoi tap do, va tra ve cau hoi da chon.
 * Nem exception invalid_argument neu tap rong.
 */
Question randomQuestionFrom(set<Question>& questions) {
    if (questions.empty()) {
        throw invalid_argument("set is empty");
    }
    Question chosen = randomElement(questions);
    questions.erase(chosen);
    return chosen;
}

/* scoresFrom
 *
 * Nhan vao map cac cau hoi va cau tra loi tuong ung (1-5),
 * tinh tong diem OCEAN cho nguoi dung.
 * Chuyen doi cau tra loi 1-5 thanh -2 den +2 truoc khi tinh.
 * Tra ve map<char, int> voi diem tung yeu to OCEAN.
 * Nhung yeu to khong co diem se khong xuat hien trong map.
 */
map<char, int> scoresFrom(map<Question, int>& answers) {
    map<char, int> scores;
    for (auto& entry : answers) {
        const Question& q = entry.first;
        int userAnswer = entry.second;  // 1 den 5
        int converted = userAnswer - 3; // quy doi thanh -2, -1, 0, 1, 2
        for (auto& factor : q.factors) {
            char trait = factor.first;
            int weight = factor.second;  // +1 hoac -1
            scores[trait] += converted * weight;
        }
    }
    // Xoa nhung yeu to co diem = 0
    for (auto it = scores.begin(); it != scores.end(); ) {
        if (it->second == 0) {
            it = scores.erase(it);
        }
        else {
            ++it;
        }
    }
    return scores;
}

/* normalize
 *
 * Nhan vao map diem nguyen (OCEAN),
 * tinh do dai vector (can bac hai cua tong binh phuong),
 * chia moi phan tu cho do dai do.
 * Tra ve map<char, double> da duoc chuan hoa.
 */
map<char, double> normalize(map<char, int>& scores) {
    double sumOfSquares = 0.0;
    for (auto& entry : scores) {
        sumOfSquares += pow((double)entry.second, 2);
    }
    double length = sqrt(sumOfSquares);

    map<char, double> result;
    for (auto& entry : scores) {
        result[entry.first] = entry.second / length;
    }
    return result;
}

/* cosineSimilarityOf
 *
 * Tinh do tuong dong cosine giua hai vector diem da chuan hoa.
 * Nhung key khong co trong mot trong hai map duoc coi la 0.
 * Su dung .at() (khong dung []) vi tham so la const.
 * Tra ve tich vo huong cua hai vector.
 */
double cosineSimilarityOf(const map<char, double>& lhs,
    const map<char, double>& rhs) {
    double dotProduct = 0.0;
    for (auto& entry : lhs) {
        char key = entry.first;
        if (rhs.count(key)) {
            dotProduct += entry.second * rhs.at(key);
        }
    }
    return dotProduct;
}

/* mostSimilarTo
 *
 * Nhan vao diem tho cua nguoi dung va tap nhan vat,
 * tra ve nhan vat co cosine similarity cao nhat voi nguoi dung.
 * Chuan hoa diem ca nguoi dung lan tung nhan vat truoc khi so sanh.
 */
Person mostSimilarTo(map<char, int>& scores, set<Person>& people) {
    map<char, double> userNorm = normalize(scores);
    Person bestMatch;
    double bestScore = lowest_double;

    for (auto person : people) {
        map<char, int> personScores = person.scores;
        map<char, double> personNorm = normalize(personScores);
        double similarity = cosineSimilarityOf(userNorm, personNorm);
        if (similarity > bestScore) {
            bestScore = similarity;
            bestMatch = person;
        }
    }
    return bestMatch;
}





// main.cpp
// Personality Tests Project
// Chuong trinh chinh: doc file, hien thi menu, chay quiz OCEAN,
// in ra nhan vat phu hop nhat voi nguoi dung.

#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "driver.h"

using namespace std;

/* parseQuestions
 *
 * Doc file questions.txt, parse tung dong thanh struct Question.
 * Dinh dang moi dong: "Question text. F1:+/-1 F2:+/-1 ..."
 * Tra ve set<Question> chua toan bo cau hoi.
 */
set<Question> parseQuestions(const string& filename) {
    set<Question> questions;
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        Question q;
        // Tim vi tri dau cua yeu to (chu hoa + dau hai cham)
        size_t factorStart = line.rfind(". ") + 2;
        // Lay phan text cau hoi (bao gom dau cham)
        q.questionText = line.substr(0, factorStart - 1);
        // Parse tung yeu to: vi du "E:1" hoac "A:-1"
        istringstream ss(line.substr(factorStart));
        string token;
        while (ss >> token) {
            char trait = token[0];
            int weight = stoi(token.substr(2));
            q.factors[trait] = weight;
        }
        questions.insert(q);
    }
    return questions;
}

/* parsePeople
 *
 * Doc file .people, parse tung dong thanh struct Person.
 * Dinh dang moi dong: "Name. O:val C:val E:val A:val N:val"
 * Tra ve set<Person> chua toan bo nhan vat.
 */
set<Person> parsePeople(const string& filename) {
    set<Person> people;
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        Person p;
        size_t dotPos = line.find(". ");
        p.name = line.substr(0, dotPos);
        istringstream ss(line.substr(dotPos + 2));
        string token;
        while (ss >> token) {
            char trait = token[0];
            int score = stoi(token.substr(2));
            p.scores[trait] = score;
        }
        people.insert(p);
    }
    return people;
}

/* selectPeopleFile
 *
 * Hien thi menu cho nguoi dung chon bo nhan vat,
 * tra ve ten file tuong ung.
 */
string selectPeopleFile() {
    vector<string> options = {
        "BabyAnimals",
        "Brooklyn99",
        "Disney",
        "Hogwarts",
        "MyersBriggs",
        "SesameStreet",
        "StarWars",
        "Vegetables",
        "mine"
    };

    cout << "Welcome to the Personality Quiz!" << endl;
    cout << "Choose a dataset to compare yourself with:" << endl;
    for (int i = 0; i < (int)options.size(); i++) {
        cout << i + 1 << ". " << options[i] << endl;
    }

    int choice = 0;
    while (choice < 1 || choice >(int)options.size()) {
        cout << "Enter your choice (1-" << options.size() << "): ";
        cin >> choice;
    }
    return options[choice - 1] + ".people";
}

/* runQuiz
 *
 * Hoi nguoi dung mot so cau hoi nhat dinh,
 * ghi lai cau tra loi vao map answers,
 * tra ve map<Question, int> cac cau da tra loi.
 */
map<Question, int> runQuiz(set<Question>& questions, int numQuestions) {
    map<Question, int> answers;
    cout << "\nAnswer each statement from 1 (Strongly Disagree) to 5 (Strongly Agree).\n" << endl;

    for (int i = 0; i < numQuestions && !questions.empty(); i++) {
        Question q = randomQuestionFrom(questions);
        cout << "Q" << (i + 1) << ": " << q.questionText << endl;

        int answer = 0;
        while (answer < 1 || answer > 5) {
            cout << "Your answer (1-5): ";
            cin >> answer;
        }
        answers[q] = answer;
        cout << endl;
    }
    return answers;
}

/* printResult
 *
 * In ra nhan vat phu hop nhat va diem OCEAN cua nguoi dung.
 */
void printResult(const Person& match, const map<char, int>& scores) {
    cout << "=============================" << endl;
    cout << "Your OCEAN scores:" << endl;
    for (auto& entry : scores) {
        cout << "  " << entry.first << ": " << entry.second << endl;
    }
    cout << "\nYou are most similar to: " << match.name << "!" << endl;
    cout << "=============================" << endl;
}

int main() {
    // Buoc 1: Doc file cau hoi (chi doc 1 lan)
    set<Question> allQuestions = parseQuestions("questions.txt");

    // Buoc 2: Cho nguoi dung chon bo nhan vat
    string peopleFile = selectPeopleFile();

    // Buoc 3: Doc file nhan vat (chi doc 1 lan)
    set<Person> people = parsePeople(peopleFile);

    // Buoc 4: Chay quiz (10 cau hoi)
    map<Question, int> answers = runQuiz(allQuestions, 10);

    // Buoc 5: Tinh diem OCEAN cua nguoi dung
    map<char, int> userScores = scoresFrom(answers);

    // Buoc 6: Tim nhan vat phu hop nhat
    Person best = mostSimilarTo(userScores, people);

    // Buoc 7: In ket qua
    printResult(best, userScores);

    // Buoc 8: Hoi co muon choi lai khong
    char again;
    cout << "\nPlay again? (y/n): ";
    cin >> again;
    if (again == 'y' || again == 'Y') {
        // Tai lai toan bo
        set<Question> freshQuestions = parseQuestions("questions.txt");
        string newFile = selectPeopleFile();
        set<Person> newPeople = parsePeople(newFile);
        map<Question, int> newAnswers = runQuiz(freshQuestions, 10);
        map<char, int> newScores = scoresFrom(newAnswers);
        Person newBest = mostSimilarTo(newScores, newPeople);
        printResult(newBest, newScores);
    }

    cout << "\nThanks for playing!" << endl;
    return 0;
}
