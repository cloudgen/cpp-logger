**file**: docs/requirements/requirement-actor-role-subject.md
**Status**: Active
**Area**: architecture
**Key**: requirement-actor-role-subject
**Updated**: 2026-11-09
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file names who starts cpp-logger. The program has no approval desk. The table has three columns: Actor, Role, and Subject.

### 1.1 Human-facing

**In one sentence:** A normal user starts cpp-logger, and nobody else has to approve that run.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The normal user who starts the program | You run `cpp-logger help` |
| The other role | The console program | It writes the log line and then exits |
| Not this file | The log line, the switches, and the build | `requirement-cpp-logger.md` |

| Includes | Excludes |
|----------|----------|
| The normal user who starts the program | An approver, a system user, and an in-application sudo grant |

| Surface | What you open | What for |
|---------|---------------|----------|
| The terminal | The command | Start the program |
| This file | The three-column table | See who that person is |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Start the program | You are the operator. The subject is the program | `cpp-logger about` |

## 2. Core rules

| Actor | Role | Subject |
|-------|------|---------|
| the normal user who starts the program | operator | cpp-logger |

1. The program does not call `sudo`. It does not create a system user. It does not open an approval desk.
2. This table has no Approver column and no Submitter column.
3. When the effective user id is 0, the log directory is still `/var/cpp-logger/log`. That path is `requirement-cpp-logger-folder.md`. It is not an approval role and it is not a sudo grant.

## 3. Protection rule

Future editors must not add an Approver column, invent an approver, or turn this file into a second class requirement.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-class-software-dev.md` | Class law that points here |
| `docs/requirements/requirement-cpp-logger-folder.md` | The two log directories |

## Terminologies

**Actor / role / subject** is the light consider catalog for a software-development program that has no dest approver: who runs the program, in what role, and for whom. The printed table has three columns: Actor, Role, and Subject. It does not grow an Approver column.

Actor / role / subject is the short who-is-who list for a kitchen that has no stamp person: who cooks, in what job, and whose cake it is. You still write the list. You do not add a fake teacher signature.

Daily-life example: a home bake has a mixer, the job "mix", and nobody else's form to stamp. You do not invent a vice-principal so the sheet looks official.

## 5. Status history

| Version | Date | Change |
|---------|------|--------|
| 1.0.0 | 2026-11-09 | Light catalog for the normal user who starts cpp-logger. No approver. |
