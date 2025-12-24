#include "TreeWidzard.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(treewidzard_py, m) {
  m.doc() = "TreeWidzard Python Bindings";

  py::class_<TreeWidzard::Engine>(m, "Engine")
      .def(py::init<>())
      .def("loadCore", &TreeWidzard::Engine::loadCore, py::arg("coreName"),
           py::arg("parameter") = 0, "Load a dynamic core plugin")
      .def("setConjecture", &TreeWidzard::Engine::setConjecture,
           py::arg("conjectureString"), "Set the conjecture to check")
      .def("setWidth", &TreeWidzard::Engine::setWidth, py::arg("width"),
           "Set the treewidth/pathwidth")
      .def("solve", &TreeWidzard::Engine::solve, "Run the algorithm");
}
