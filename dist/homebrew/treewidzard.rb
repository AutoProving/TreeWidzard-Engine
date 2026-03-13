class Treewidzard < Formula
  desc "TreeWidzard: A specialized tree decomposition tool"
  homepage "https://github.com/AutoProving/TreeWidzard-Engine"
  license "MIT"
  head "https://github.com/AutoProving/TreeWidzard-Engine.git", branch: "main"

  depends_on "cmake" => :build
  depends_on "bison" => :build
  depends_on "flex" => :build
  def install
    system "cmake", "-S", ".", "-B", "build", *std_cmake_args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    system "#{bin}/treewidzard", "--help"
  end
end
