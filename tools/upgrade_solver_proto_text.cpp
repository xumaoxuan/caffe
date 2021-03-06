/*
All modification made by Cambricon Corporation: © 2018-2019 Cambricon Corporation
All rights reserved.
All other contributions:
Copyright (c) 2014--2019, the respective contributors
All rights reserved.
For the list of contributors go to https://github.com/BVLC/caffe/blob/master/CONTRIBUTORS.md
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// This is a script to upgrade old solver prototxts to the new format.
// Usage:
//    upgrade_solver_proto_text old_solver_proto_file_in solver_proto_file_out

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "caffe/util/io.hpp"
#include "caffe/util/upgrade_proto.hpp"

using std::ofstream;

using namespace caffe;  // NOLINT(build/namespaces)

int main(int argc, char** argv) {
  FLAGS_alsologtostderr = 1;  // Print output to stderr (while still logging)
  ::google::InitGoogleLogging(argv[0]);
  if (argc != 3) {
    LOG(ERROR) << "Usage: " << argv[0]
        << "old_solver_proto_file_in solver_proto_file_out";
    return 1;
  }

  SolverParameter solver_param;
  string input_filename(argv[1]);
  if (!ReadProtoFromTextFile(input_filename, &solver_param)) {
    LOG(ERROR) << "Failed to parse input text file as SolverParameter: "
               << input_filename;
    return 2;
  }
  bool need_upgrade = SolverNeedsTypeUpgrade(solver_param);
  bool success = true;
  if (need_upgrade) {
    success = UpgradeSolverAsNeeded(input_filename, &solver_param);
    if (!success) {
      LOG(ERROR) << "Encountered error(s) while upgrading prototxt; "
                 << "see details above.";
      return 3;
    }
  } else {
    LOG(ERROR) << "File already in latest proto format: " << input_filename;
    return 4;
  }

  // Save new format prototxt.
  WriteProtoToTextFile(solver_param, argv[2]);

  LOG(INFO) << "Wrote upgraded SolverParameter text proto to " << argv[2];
  return 0;
}
