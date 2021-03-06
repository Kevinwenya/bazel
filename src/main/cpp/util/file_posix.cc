// Copyright 2014 The Bazel Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "src/main/cpp/util/file_posix.h"

#include <sys/stat.h>
#include <stdlib.h>  // getenv
#include <unistd.h>  // access
#include <vector>

#include "src/main/cpp/util/file.h"
#include "src/main/cpp/util/exit_code.h"
#include "src/main/cpp/util/errors.h"
#include "src/main/cpp/util/strings.h"

using std::pair;

namespace blaze_util {

string Which(const string &executable) {
  char *path_cstr = getenv("PATH");
  if (path_cstr == NULL || path_cstr[0] == '\0') {
    die(blaze_exit_code::LOCAL_ENVIRONMENTAL_ERROR,
        "Could not get PATH to find %s", executable.c_str());
  }

  string path(path_cstr);
  std::vector<std::string> pieces = blaze_util::Split(path, ':');
  for (auto piece : pieces) {
    if (piece.empty()) {
      piece = ".";
    }

    struct stat file_stat;
    string candidate = blaze_util::JoinPath(piece, executable);
    if (access(candidate.c_str(), X_OK) == 0 &&
        stat(candidate.c_str(), &file_stat) == 0 &&
        S_ISREG(file_stat.st_mode)) {
      return candidate;
    }
  }
  return "";
}

}  // namespace blaze_util
