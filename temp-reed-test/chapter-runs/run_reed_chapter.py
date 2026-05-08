#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import shutil
import subprocess
from collections import Counter
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable, Optional


SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parents[1]
PROPERTIES_DIR = SCRIPT_DIR / "properties"
DEFAULT_RUNS_DIR = SCRIPT_DIR / "runs"
LAUNCHER = REPO_ROOT / "treewidzard.sh"


@dataclass(frozen=True)
class RunSpec:
    id: str
    workspace_id: str
    group: str
    mode: str
    description: str
    property_file: str
    width_kind: Optional[str]
    width_value: Optional[int]
    strategy: Optional[str]
    premise: bool
    files: bool
    threads: Optional[int]
    expected_verdict: str
    historical_status: str
    notes: str


def property_for_delta(delta: int) -> str:
    if delta == 2:
        return "reed_delta2.txt"
    if delta == 3:
        return "reed_delta3.txt"
    if delta == 4:
        return "reed_delta4.txt"
    raise ValueError(f"unsupported delta {delta}")


def pathwidth_historical_status(strategy: str, premise: bool, width: int,
                                delta: int) -> str:
    if strategy == "BreadthFirstSearch" and not premise:
      if width == 2:
        return "completed"
      if width == 3 and delta in {2, 3}:
        return "memory_limit"
      if width == 3 and delta == 4:
        return "completed"
      if width == 4:
        return "memory_limit"
    if strategy == "BreadthFirstSearch" and premise:
      return "completed"
    if strategy == "IsomorphismBreadthFirstSearch" and not premise:
      if width in {2, 3}:
        return "completed"
      if width == 4:
        return "memory_limit"
    if strategy == "IsomorphismBreadthFirstSearch" and premise:
      return "completed"
    raise ValueError("unexpected pathwidth configuration")


def treewidth_historical_status(premise: bool, width: int, delta: int) -> str:
    if not premise:
      return "timeout"
    if width in {2, 3}:
      return "completed"
    if width == 4 and delta == 2:
      return "completed"
    if width == 4 and delta in {3, 4}:
      return "timeout"
    raise ValueError("unexpected treewidth configuration")


def build_runs() -> list[RunSpec]:
    runs: list[RunSpec] = []

    for width in (2, 3, 4):
        for delta in (2, 3, 4):
            for strategy, premise in (
                ("BreadthFirstSearch", False),
                ("BreadthFirstSearch", True),
                ("IsomorphismBreadthFirstSearch", False),
                ("IsomorphismBreadthFirstSearch", True),
            ):
                suffix = strategy.replace("Search", "").lower()
                premise_suffix = "_premise" if premise else ""
                runs.append(
                    RunSpec(
                        id=f"pw{width}_d{delta}_{suffix}{premise_suffix}",
                        workspace_id=f"pw{width}_d{delta}_{suffix}{premise_suffix}",
                        group="pathwidth-ablation",
                        mode="atp",
                        description=(
                            f"Pathwidth {width}, degree bound {delta}, "
                            f"{strategy}{' + premise' if premise else ''}"
                        ),
                        property_file=property_for_delta(delta),
                        width_kind="pw",
                        width_value=width,
                        strategy=strategy,
                        premise=premise,
                        files=False,
                        threads=None,
                        expected_verdict="sat",
                        historical_status=pathwidth_historical_status(
                            strategy, premise, width, delta),
                        notes="Chapter pathwidth ablation grid.",
                    ))

    for width in (2, 3, 4):
        for delta in (2, 3, 4):
            for premise in (False, True):
                premise_suffix = "_premise" if premise else ""
                runs.append(
                    RunSpec(
                        id=f"tw{width}_d{delta}_iso{premise_suffix}",
                        workspace_id=f"tw{width}_d{delta}_iso{premise_suffix}",
                        group="treewidth-ablation",
                        mode="atp",
                        description=(
                            f"Treewidth {width}, degree bound {delta}, "
                            f"IsomorphismBreadthFirstSearch"
                            f"{' + premise' if premise else ''}"
                        ),
                        property_file=property_for_delta(delta),
                        width_kind="tw",
                        width_value=width,
                        strategy="IsomorphismBreadthFirstSearch",
                        premise=premise,
                        files=False,
                        threads=None,
                        expected_verdict="sat",
                        historical_status=treewidth_historical_status(
                            premise, width, delta),
                        notes="Chapter treewidth summary grid.",
                    ))

    runs.append(
        RunSpec(
            id="pw5_d3_parallel_iso_premise_128t",
            workspace_id="pw5_d3_parallel_iso_premise_128t",
            group="largest-confirmation",
            mode="atp",
            description=(
                "Largest published Reed confirmation run: pathwidth 5, "
                "degree bound 3, combined parallel canonization + premise"
            ),
            property_file="reed_delta3.txt",
            width_kind="pw",
            width_value=5,
            strategy="ParallelIsomorphismBreadthFirstSearch",
            premise=True,
            files=False,
            threads=128,
            expected_verdict="sat",
            historical_status="completed",
            notes=(
                "Mapped to the current repo's parallel canonized search. "
                "Historically reported as 64 cores / 128 threads."
            ),
        ))

    counterexamples = (
        RunSpec(
            id="pw2_delta2_to_2color_counterexample_atp",
            workspace_id="pw2_delta2_to_2color_counterexample",
            group="counterexample-atp",
            mode="atp",
            description=(
                "Prose counterexample run: under Delta <= 2, demanding "
                "2-colourability should fail (C5 obstruction)."
            ),
            property_file="reed_delta2_to_2color.txt",
            width_kind="pw",
            width_value=2,
            strategy="IsomorphismBreadthFirstSearch",
            premise=True,
            files=True,
            threads=None,
            expected_verdict="unsat",
            historical_status="prose-counterexample",
            notes=(
                "Reconstructed from chapter prose. Uses pathwidth 2 to witness "
                "the C5 obstruction explicitly."
            ),
        ),
        RunSpec(
            id="pw4_triangle_free_3color_counterexample_atp",
            workspace_id="pw4_triangle_free_3color_counterexample",
            group="counterexample-atp",
            mode="atp",
            description=(
                "Figure-left counterexample: pathwidth 4 triangle-free graph "
                "need not be 3-colourable."
            ),
            property_file="triangle_free_3colorable.txt",
            width_kind="pw",
            width_value=4,
            strategy="IsomorphismBreadthFirstSearch",
            premise=True,
            files=True,
            threads=None,
            expected_verdict="unsat",
            historical_status="prose-counterexample",
            notes="Matches the left-hand counterexample workflow from the chapter.",
        ),
        RunSpec(
            id="pw4_delta4_to_3color_counterexample_atp",
            workspace_id="pw4_delta4_to_3color_counterexample",
            group="counterexample-atp",
            mode="atp",
            description=(
                "Figure-right counterexample: pathwidth 4, triangle-free, "
                "Delta <= 4 does not force 3-colourability."
            ),
            property_file="reed_delta4_to_3color.txt",
            width_kind="pw",
            width_value=4,
            strategy="IsomorphismBreadthFirstSearch",
            premise=True,
            files=True,
            threads=None,
            expected_verdict="unsat",
            historical_status="prose-counterexample",
            notes="Matches the right-hand counterexample workflow from the chapter.",
        ),
    )
    runs.extend(counterexamples)

    for atp_run in counterexamples:
        runs.append(
            RunSpec(
                id=atp_run.id.replace("_atp", "_modelcheck"),
                workspace_id=atp_run.workspace_id,
                group="counterexample-recheck",
                mode="modelcheck_itd",
                description=f"PACE model-checking recheck for {atp_run.workspace_id}.",
                property_file=atp_run.property_file,
                width_kind=atp_run.width_kind,
                width_value=atp_run.width_value,
                strategy=atp_run.strategy,
                premise=atp_run.premise,
                files=False,
                threads=None,
                expected_verdict="unsat",
                historical_status="recheck",
                notes="Rechecks the exported counterexample independently of ATP.",
            ))

    return runs


def property_copy_path(workspace: Path, run: RunSpec) -> Path:
    return workspace / Path(run.property_file).name


def width_name(kind: str) -> str:
    return "tree_width" if kind == "tw" else "path_width"


def counterexample_base(workspace: Path, run: RunSpec) -> Path:
    property_path = property_copy_path(workspace, run)
    premise_suffix = "_Premise" if run.premise else ""
    return workspace / (
        f"{property_path.stem}_{run.strategy}_{width_name(run.width_kind)}_"
        f"{run.width_value}{premise_suffix}_CounterExample"
    )


def build_atp_command(run: RunSpec, property_name: str,
                      threads_override: Optional[int]) -> list[str]:
    command = [
        str(LAUNCHER),
        "-atp",
        f"{run.width_kind}={run.width_value}",
        "-pl",
    ]

    if run.premise:
        command.append("-premise")
    if run.files:
        command.append("-files")

    threads = run.threads
    if threads_override is not None and threads is not None:
        threads = threads_override
    if threads is not None:
        command.extend(["-nthreads", str(threads)])

    command.extend([run.strategy, property_name])
    return command


def build_modelcheck_command(workspace: Path, run: RunSpec,
                             property_name: str) -> list[str]:
    base = counterexample_base(workspace, run)
    if run.strategy == "IsomorphismBreadthFirstSearch":
        decomposition = Path(str(base) + "_InstructiveDecomposition.txt")
    else:
        decomposition = Path(str(base) + "_ITD.txt")
    if not decomposition.exists():
        raise FileNotFoundError(
            f"missing instructive decomposition artifact: {decomposition}")

    return [
        str(LAUNCHER),
        "-modelcheck",
        "ITD",
        property_name,
        decomposition.name,
    ]


def command_preview(run: RunSpec, threads_override: Optional[int]) -> str:
    property_name = Path(run.property_file).name
    if run.mode == "atp":
        return " ".join(build_atp_command(run, property_name, threads_override))

    premise_suffix = "_Premise" if run.premise else ""
    base = (
        f"{Path(run.property_file).stem}_{run.strategy}_"
        f"{width_name(run.width_kind)}_{run.width_value}{premise_suffix}_CounterExample"
    )
    if run.strategy == "IsomorphismBreadthFirstSearch":
        decomposition = base + "_InstructiveDecomposition.txt"
    else:
        decomposition = base + "_ITD.txt"
    return " ".join([
        str(LAUNCHER),
        "-modelcheck",
        "ITD",
        property_name,
        decomposition,
    ])


def select_runs(runs: Iterable[RunSpec], groups: list[str],
                ids: list[str]) -> list[RunSpec]:
    selected = list(runs)
    if groups:
        allowed = set(groups)
        selected = [run for run in selected if run.group in allowed]
    if ids:
        allowed = set(ids)
        selected = [run for run in selected if run.id in allowed]
    return selected


def print_summary(runs: list[RunSpec]) -> None:
    print(f"Repo root: {REPO_ROOT}")
    print(f"Launcher:  {LAUNCHER}")
    print(f"Runs:      {len(runs)}")
    counts = Counter(run.group for run in runs)
    for group, count in sorted(counts.items()):
        print(f"  {group}: {count}")


def print_list(runs: list[RunSpec], threads_override: Optional[int]) -> None:
    for run in runs:
        print(
            "\t".join([
                run.id,
                run.group,
                run.expected_verdict,
                run.historical_status,
                command_preview(run, threads_override),
            ]))


def print_json(runs: list[RunSpec], threads_override: Optional[int]) -> None:
    payload = []
    for run in runs:
        record = asdict(run)
        record["command_preview"] = command_preview(run, threads_override)
        payload.append(record)
    print(json.dumps(payload, indent=2))


def ensure_workspace(workspace: Path, run: RunSpec) -> Path:
    workspace.mkdir(parents=True, exist_ok=True)
    source_property = PROPERTIES_DIR / run.property_file
    if not source_property.exists():
        raise FileNotFoundError(f"missing property file: {source_property}")
    destination = property_copy_path(workspace, run)
    if not destination.exists():
        shutil.copy2(source_property, destination)
    return destination


def run_one(run: RunSpec, runs_root: Path, overwrite: bool,
            threads_override: Optional[int]) -> int:
    workspace = runs_root / run.workspace_id
    property_path = ensure_workspace(workspace, run)

    out_path = workspace / f"{run.id}.out"
    err_path = workspace / f"{run.id}.err"
    if out_path.exists() and err_path.exists() and not overwrite:
        print(f"[skip] {run.id} (outputs already exist)")
        return 0

    if run.mode == "atp":
        command = build_atp_command(run, property_path.name, threads_override)
    elif run.mode == "modelcheck_itd":
        command = build_modelcheck_command(workspace, run, property_path.name)
    else:
        raise ValueError(f"unsupported mode {run.mode}")

    print(f"[run] {run.id}")
    print(f"      {workspace}")
    print(f"      {' '.join(command)}")

    with out_path.open("w", encoding="utf-8") as out_f, err_path.open(
            "w", encoding="utf-8") as err_f:
        completed = subprocess.run(
            command,
            cwd=workspace,
            stdout=out_f,
            stderr=err_f,
            check=False,
            text=True,
        )

    print(f"[done] {run.id} exit={completed.returncode}")
    return completed.returncode


def main() -> int:
    parser = argparse.ArgumentParser(
        description="List or execute the reconstructed Reed chapter runs.")
    parser.add_argument("--summary", action="store_true",
                        help="Print only a grouped run summary.")
    parser.add_argument("--list", action="store_true",
                        help="Print the selected runs in tab-separated form.")
    parser.add_argument("--json", action="store_true",
                        help="Print the selected runs as JSON.")
    parser.add_argument("--run", action="store_true",
                        help="Execute the selected runs.")
    parser.add_argument("--group", action="append", default=[],
                        help="Restrict to one group (repeatable).")
    parser.add_argument("--id", action="append", default=[],
                        help="Restrict to one run id (repeatable).")
    parser.add_argument("--runs-root", type=Path, default=DEFAULT_RUNS_DIR,
                        help=f"Workspace root for run outputs (default: {DEFAULT_RUNS_DIR})")
    parser.add_argument("--overwrite", action="store_true",
                        help="Rerun even if output files already exist.")
    parser.add_argument("--threads-override", type=int, default=None,
                        help="Override thread count for runs that already specify one.")
    args = parser.parse_args()

    runs = select_runs(build_runs(), args.group, args.id)
    if not runs:
        print("No runs selected.")
        return 1

    if args.summary or not (args.list or args.json or args.run):
        print_summary(runs)

    if args.list:
        print_list(runs, args.threads_override)

    if args.json:
        print_json(runs, args.threads_override)

    if args.run:
        exit_codes = [
            run_one(run, args.runs_root, args.overwrite, args.threads_override)
            for run in runs
        ]
        return max(exit_codes, default=0)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
