#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

using namespace std;

std::vector<std::vector<float>> fp_gen(int n, int m) {
  int i = 0;
  std::vector<std::vector<float>> fp(n, std::vector<float>(m, 0));
  struct timespec start, end;
  std::hash<std::string> hash_fnc;
  std::string seed =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz012345678+-*/=!?";
  while (i < m) {
    int j = 0;
    while (j < n) {
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
      hash_fnc(seed + std::to_string(j));
      seed.compare(seed + std::to_string(j));
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
      float t = end.tv_nsec - start.tv_nsec;
      fp[j][i] = t;
      j++;
    }
    i++;
  }
  return fp;
}

float cal_mode(const std::vector<float> &in) {
  int max_cnt = 0;
  float mode = 0;
  for (int i = 0; i < in.size(); ++i) {
    int cnt = count(in.begin(), in.end(), in[i]);
    if (cnt > max_cnt) {
      max_cnt = cnt;
      mode = in[i];
    }
  }
  return mode;
}

int num_coindences(const std::vector<std::vector<float>> &fp1,
                   const std::vector<std::vector<float>> &fp2, int n, int m) {
  int num_c = 0;
  std::vector<float> fp1_mode;
  int i = 0;
  while (i < n) {
    float mode = cal_mode(fp1[i]);
    fp1_mode.push_back(mode);
    i++;
  }

  i = 0;
  while (i < n) {
    bool check = false;
    int j = 0;
    while (j < m && !check) {
      if (fp1_mode[i] == fp2[i][j]) {
        num_c++;
        check = true;
      } else {
        j++;
      }
    }
    i++;
  }

  return num_c;
}

float fp_sim(const std::vector<std::vector<float>> &fp1,
             const std::vector<std::vector<float>> &fp2, int n, int m) {
  int num_c = num_coindences(fp1, fp2, n, m);
  num_c += num_c + num_coindences(fp2, fp1, n, m);
  float sim = (float)num_c / (n * 2);
  return sim;
}

void write_fp(const std::vector<std::vector<float>> &fp, std::string file) {
  ofstream out(file, std::ios::out);
  if (out.is_open()) {
    for (int i = 0; i < fp.size(); ++i) {
      for (int j = 0; j < fp[i].size(); ++j) {
        out << fp[i][j] << endl;
      }
    }
    out.close();
  }
}

std::vector<std::vector<float>> read_fp(std::string file, int n, int m) {
  std::vector<std::vector<float>> fp(n, std::vector<float>(m, 0));
  ifstream in(file, std::ios::out);
  if (in.is_open()) {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
        float tmp;
        in >> tmp;
        fp[i][j] = tmp;
      }
    }
    in.close();
  }
  return fp;
}

int main(int argc, char **argv) {
  printf("ver:%s %s\n", __DATE__, __TIME__);
  int n = 2000, m = 100;
  std::string cmd = argc > 1 ? argv[1] : "";
  std::string file = argc > 1 ? argv[2] : "";
  if (cmd == "-o" || cmd == "--output") {
    std::vector<std::vector<float>> fp = fp_gen(n, m);
    write_fp(fp, file);
    return 0;
  }
  if (cmd == "-c" || cmd == "--check") {
    std::vector<std::vector<float>> fp_in = read_fp(file, n, m);
    std::vector<std::vector<float>> fp_local = fp_gen(n, m);
    float sim = fp_sim(fp_in, fp_local, n, m);
    cout << "sim:" << sim << endl;
    return 0;
  }
  printf(">> usage: %s [opt] [file]\n",argv[0]);
  printf(">> -o --output // save fingerprint\n");
  printf(">> -c --check  // check fingerprint\n");
  std::vector<std::vector<float>> fp1 = fp_gen(n, m);
  usleep(1000 * 1000 * 2);
  std::vector<std::vector<float>> fp2 = fp_gen(n, m);
  float sim = fp_sim(fp1, fp2, n, m);
  cout << "sim:" << sim << endl;
  /*
    for (int i = 0; i < fp1.size(); ++i) {
      for (int j = 0; j < fp1[i].size(); ++j) {
        cout << fp1[i][j] << " ";
      }
      cout<<"   mode:"<<cal_mode(fp1[i]);
      cout << endl;
    }
  */
  return 0;
}
