"""노트북 02~04가 함께 쓰는 가짜 배 시뮬레이션.

알고리즘(외삽, 칼만 필터)은 노트북에 있고, 여기에는 "정답 항적"과 "AIS 보고"를 만드는 코드만 둔다.
좌표는 출발점 기준 국지 평면(m): e = 동쪽, n = 북쪽. 방향은 북쪽 0°, 시계 방향.
"""

import numpy as np

KN = 1852 / 3600  # 1노트 = 0.5144 m/s

SPEED_KN = 12.0              # 배 속력
REPORT_INTERVAL = 20.0       # AIS 보고 간격 (초)
DURATION = 600.0             # 시뮬레이션 길이 (초)
TURN_START, TURN_END = 200.0, 380.0
TURN_RATE = 1.0              # 선회 중 침로가 도는 속도 (°/초)

POS_NOISE = 5.0              # 위치 오차 표준편차 (m)
SOG_NOISE = 0.2              # 속력 오차 (kn)
COG_NOISE = 2.0              # 침로 오차 (°)

# 그림을 확대해 볼 범위 ((동쪽 min, max), (북쪽 min, max)): 직진 구간, 선회 구간
ZOOM_VIEWS = [((-30, 30), (500, 1250)), ((-50, 780), (1150, 1650))]


def turn_rate_at(t):
    """t 초에 침로가 도는 속도 (°/초). 200~380초에만 1°/초로 우회전."""
    t = np.asarray(t, dtype=float)
    return np.where((t >= TURN_START) & (t < TURN_END), TURN_RATE, 0.0)


def true_track(t):
    """t 초(배열)의 실제 위치와 침로. (e, n, course°) 배열을 돌려준다."""
    dt = 0.05
    ts = np.arange(0, DURATION + REPORT_INTERVAL + dt, dt)
    v = SPEED_KN * KN
    course = np.cumsum(np.r_[0, turn_rate_at(ts[:-1]) * dt])  # °
    rad = np.radians(course)
    e = np.cumsum(np.r_[0, v * np.sin(rad[:-1]) * dt])
    n = np.cumsum(np.r_[0, v * np.cos(rad[:-1]) * dt])
    i = np.round(np.asarray(t, dtype=float) / dt).astype(int)
    return e[i], n[i], course[i] % 360


def make_reports(seed=1):
    """REPORT_INTERVAL 마다 오는 AIS 보고. 위치·SOG·COG에 잡음이 섞여 있다.

    돌려주는 값: dict(t, e, n, sog, cog)
    """
    rng = np.random.default_rng(seed)
    t = np.arange(0, DURATION + 1, REPORT_INTERVAL)
    e, n, course = true_track(t)
    return {
        "t": t,
        "e": e + rng.normal(0, POS_NOISE, len(t)),
        "n": n + rng.normal(0, POS_NOISE, len(t)),
        "sog": SPEED_KN + rng.normal(0, SOG_NOISE, len(t)),
        "cog": (course + rng.normal(0, COG_NOISE, len(t))) % 360,
    }


def is_turning(t):
    return TURN_START <= t < TURN_END


def evaluate(reports, predict, step=0.5):
    """보고 k 를 받은 뒤 다음 보고 전까지 predict(k, tau) 로 그린 위치가 실제와 평균 몇 m 틀리나.

    predict(k, tau) -> (e, n) 배열. tau 는 보고 k 이후 지난 초(배열).
    돌려주는 값: {"직진": 평균 오차, "선회": 평균 오차}
    """
    taus = np.arange(0, REPORT_INTERVAL, step)
    errors = {"직진": [], "선회": []}
    for k, t0 in enumerate(reports["t"][:-1]):
        te, tn, _ = true_track(t0 + taus)
        pe, pn = predict(k, taus)
        errors["선회" if is_turning(t0) else "직진"].append(np.hypot(pe - te, pn - tn).mean())
    return {name: float(np.mean(v)) for name, v in errors.items()}


def plot_extrapolation(ax, reports, predict, label, color, step=0.5):
    """보고마다 다음 보고 전까지 외삽한 선을 끊어서 그린다(화면에 그려지는 모습)."""
    taus = np.arange(0, REPORT_INTERVAL + step, step)
    for k in range(len(reports["t"]) - 1):
        pe, pn = predict(k, taus)
        ax.plot(pe, pn, "-", color=color, lw=1.5, label=label if k == 0 else None)


def plot_truth(ax, reports):
    t = np.arange(0, DURATION + 0.5, 0.5)
    e, n, _ = true_track(t)
    ax.plot(e, n, "-", color="lightgray", lw=4, label="실제 항적", zorder=0)
    ax.plot(reports["e"], reports["n"], "o", color="k", ms=4, label="AIS 보고")
    ax.set_xlabel("동쪽 (m)")
    ax.set_ylabel("북쪽 (m)")
    ax.set_aspect("equal")
    ax.grid(alpha=0.3)
