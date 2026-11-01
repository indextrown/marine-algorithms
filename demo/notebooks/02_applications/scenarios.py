"""응용 4가 쓰는 시나리오 9가지와 수신 조건 3가지.

sim.py 는 "직진 → 선회 한 번" 하나만 만든다. 여기서는 배가 하는 여러 운동을 만들고,
AIS 보고 간격도 ITU-R M.1371 의 Class A·B 규칙처럼 속력·선회에 따라 바꾼다.
좌표는 국지 평면(m): e = 동쪽, n = 북쪽. 방향은 북쪽 0°, 시계 방향.
"""

import numpy as np

KN = 1852 / 3600
DURATION = 600.0
DT = 0.1  # 참값을 만드는 시간 간격 (초)


# ── 운동 ────────────────────────────────────────────────────────────────
# 대부분은 "선수 방향이 도는 속도(°/초)"와 "속력(kn)"으로 배를 민다.
# 옆으로 밀리는 속도(m/s, 우현 +)가 있으면 진행 방향(COG)이 선수 방향과 벌어진다.

def _between(t, a, b):
    return (t >= a) & (t < b)


def _zigzag_rate(t):
    """100~580초에 ±20°로 지그재그. 20초 우회전 → 40초 좌회전 → 20초 우회전을 되풀이."""
    s = (t - 100) % 80
    rate = np.where((s < 20) | (s >= 60), 1.0, -1.0)
    return np.where(_between(t, 100, 580), rate, 0.0)


def _turning_speed(t):
    """15노트로 오다 전타하면 속력이 9노트까지 떨어진다."""
    return np.where(t < 150, 15.0, 9.0 + 6.0 * np.exp(-(t - 150) / 40))


SCENARIOS = {
    "직진": dict(
        course0=30, speed=lambda t: 12.0 + 0 * t, rate=lambda t: 0 * t),
    "변침": dict(
        course0=0, speed=lambda t: 12.0 + 0 * t, rate=lambda t: np.where(_between(t, 200, 230), 1.5, 0.0)),
    "완만한 선회": dict(
        course0=0, speed=lambda t: 10.0 + 0 * t, rate=lambda t: np.where(_between(t, 150, 450), 0.2, 0.0)),
    "전타 선회": dict(
        course0=0, speed=_turning_speed, rate=lambda t: np.where(_between(t, 150, 330), 2.0, 0.0)),
    "지그재그": dict(
        course0=0, speed=lambda t: 12.0 + 0 * t, rate=_zigzag_rate),
    "저속 선회": dict(
        course0=0, speed=lambda t: 3.0 + 0 * t, rate=lambda t: np.where(_between(t, 150, 330), 0.5, 0.0),
        lateral=lambda t: np.where(_between(t, 150, 330), 0.6, 0.0)),
    "계류": dict(at_anchor=True, position=lambda t: (
        0.3 * np.sin(2 * np.pi * t / 23),           # 옆으로 조금
        1.0 * np.sin(2 * np.pi * t / 40))),         # 앞뒤로 펜더 위에서
    "묘박": dict(at_anchor=True, position=lambda t: (
        90 * np.sin(np.radians(20) * np.sin(2 * np.pi * t / 400)),   # 닻에서 90m, 좌우 ±20°로 흔들림
        -90 * np.cos(np.radians(20) * np.sin(2 * np.pi * t / 400)))),
    "황천 직진": dict(
        course0=0, speed=lambda t: 10.0 + 1.0 * np.sin(2 * np.pi * t / 9),
        rate=lambda t: 6 * (2 * np.pi / 12) * np.cos(2 * np.pi * t / 12)),  # 선수 ±6°, 12초 주기
}


def true_track(name, t_end=DURATION + 200):
    """시나리오의 참값을 DT 간격으로 만든다. dict(t, e, n, sog, cog, at_anchor)."""
    sc = SCENARIOS[name]
    t = np.arange(0, t_end + DT, DT)
    if "position" in sc:
        e, n = sc["position"](t)
        ve, vn = np.gradient(e, DT), np.gradient(n, DT)
    else:
        psi = np.radians(sc["course0"] + np.cumsum(np.r_[0, sc["rate"](t[:-1]) * DT]))
        u = sc["speed"](t) * KN
        w = sc.get("lateral", lambda t: 0 * t)(t)
        ve = u * np.sin(psi) + w * np.cos(psi)
        vn = u * np.cos(psi) - w * np.sin(psi)
        e = np.cumsum(np.r_[0, ve[:-1] * DT])
        n = np.cumsum(np.r_[0, vn[:-1] * DT])
    return dict(t=t, e=e, n=n, sog=np.hypot(ve, vn) / KN,
                cog=np.degrees(np.arctan2(ve, vn)) % 360, at_anchor=sc.get("at_anchor", False))


def truth_at(track, t):
    i = np.round(np.asarray(t, dtype=float) / DT).astype(int)
    return track["e"][i], track["n"][i]


# ── 보고 간격 (ITU-R M.1371 Table 1을 단순화) ─────────────────────────────

def _cog_rate(track):
    """진행 방향이 도는 속도 (°/초)."""
    d = np.diff(np.unwrap(np.radians(track["cog"])), append=np.nan)
    return np.degrees(d) / DT


def interval_class_a(sog, turning, at_anchor):
    if at_anchor and sog <= 3:
        return 180.0
    if sog > 23:
        return 2.0
    if sog > 14:
        return 2.0 if turning else 6.0
    return 10 / 3 if turning else 10.0


def interval_class_b(sog, turning, at_anchor):
    return 30.0 if sog > 2 else 180.0


CONDITIONS = {
    "기본 · Class A": dict(interval=interval_class_a, pos_std=5.0, loss=0.0, outlier=0.0),
    "열악 · Class A": dict(interval=interval_class_a, pos_std=10.0, loss=0.2, outlier=0.03),
    "기본 · Class B": dict(interval=interval_class_b, pos_std=5.0, loss=0.0, outlier=0.0),
}
TURNING_THRESHOLD = 0.1  # 이 이상(°/초)으로 진행 방향이 돌면 "침로를 바꾸는 중"


def make_reports(track, condition, seed=1):
    """조건에 맞춰 AIS 보고를 만든다. dict(t, e, n, sog, cog, pos_std, sog_std, cog_std).

    - 보고 간격은 그 순간의 속력·선회에 따라 정한다.
    - 위치·SOG·COG에 잡음을 섞는다. 느릴수록 COG가 많이 흔들린다.
    - 열악한 조건에서는 보고가 빠지기도 하고(손실), 위치가 크게 튀기도 한다(이상치).
    """
    cond = CONDITIONS[condition]
    rng = np.random.default_rng(seed)
    rate = _cog_rate(track)
    times, t = [], 0.0
    while t <= DURATION:
        i = int(round(t / DT))
        times.append(t)
        t += cond["interval"](track["sog"][i], abs(rate[i]) >= TURNING_THRESHOLD, track["at_anchor"])
    times = np.array(times)
    times = times[(times == 0) | (rng.random(len(times)) >= cond["loss"])]

    i = np.round(times / DT).astype(int)
    m = len(times)
    sog = track["sog"][i]
    cog_std = 2.0 + 5.0 / np.maximum(sog, 0.3)
    e = track["e"][i] + rng.normal(0, cond["pos_std"], m)
    n = track["n"][i] + rng.normal(0, cond["pos_std"], m)
    jump = rng.random(m) < cond["outlier"]
    jump[0] = False
    angle = rng.uniform(0, 2 * np.pi, m)
    e += np.where(jump, 300 * np.sin(angle), 0.0)
    n += np.where(jump, 300 * np.cos(angle), 0.0)
    return dict(
        t=times, e=e, n=n,
        sog=np.maximum(sog + rng.normal(0, 0.2, m), 0.0),
        cog=(track["cog"][i] + rng.normal(0, 1, m) * cog_std) % 360,
        pos_std=np.full(m, cond["pos_std"]), sog_std=np.full(m, 0.2), cog_std=cog_std,
    )


# ── 평가 ────────────────────────────────────────────────────────────────

def errors(track, reports, predict, step=1.0):
    """보고 k 를 받은 뒤 다음 보고까지 predict(k, tau) 로 그린 위치의 오차(m)를 모두 모은다."""
    out = []
    for k in range(len(reports["t"]) - 1):
        t0 = reports["t"][k]
        taus = np.arange(0, reports["t"][k + 1] - t0, step)
        te, tn = truth_at(track, t0 + taus)
        pe, pn = predict(k, taus)
        out.append(np.hypot(pe - te, pn - tn))
    return np.concatenate(out)
