"""응용 1~3에서 만든 세 방법을 응용 4에서 함께 쓰려고 모아 둔 것.

노트북 버전과 같은 식이고, 측정 잡음(R)만 보고마다 달라질 수 있게 reports 의 *_std 를 쓴다.
각 함수는 reports 를 받아 predict(k, tau) -> (e, n) 을 돌려준다.
"""

import numpy as np

KN = 1852 / 3600


def dead_reckoning(rep):
    """응용 1: 보고 k 의 SOG·COG 로 곧게 민다."""
    def predict(k, tau):
        v, c = rep["sog"][k] * KN, np.radians(rep["cog"][k])
        return rep["e"][k] + v * np.sin(c) * tau, rep["n"][k] + v * np.cos(c) * tau
    return predict


def kalman_cv(rep, q=0.05):
    """응용 2: 등속 칼만 필터로 다듬은 위치·속도로 곧게 민다."""
    H = np.eye(4)
    sp = rep["pos_std"][0]
    x = np.array([rep["e"][0], rep["n"][0], 0.0, 0.0])
    P = np.diag([sp**2, sp**2, 10.0, 10.0])
    states = []
    for k in range(len(rep["t"])):
        if k > 0:
            dt = rep["t"][k] - rep["t"][k - 1]
            F = np.eye(4); F[0, 2] = F[1, 3] = dt
            G = np.array([[dt**2 / 2, 0], [0, dt**2 / 2], [dt, 0], [0, dt]])
            x = F @ x
            P = F @ P @ F.T + q * G @ G.T
        v, c = rep["sog"][k] * KN, np.radians(rep["cog"][k])
        z = np.array([rep["e"][k], rep["n"][k], v * np.sin(c), v * np.cos(c)])
        sp = rep["pos_std"][k]
        sv = np.hypot(rep["sog_std"][k] * KN, v * np.radians(rep["cog_std"][k]))
        R = np.diag([sp**2, sp**2, sv**2, sv**2])
        K = P @ H.T @ np.linalg.inv(H @ P @ H.T + R)
        x = x + K @ (z - H @ x)
        P = (np.eye(4) - K @ H) @ P
        states.append(x.copy())
    states = np.array(states)

    def predict(k, tau):
        e, n, ve, vn = states[k]
        return e + ve * tau, n + vn * tau
    return predict


def move(x, dt):
    """등속 선회 모델 (북쪽 0, 시계 방향). x = [e, n, v, psi, omega]. dt 는 배열이어도 된다."""
    e, n, v, psi, w = x
    dt = np.asarray(dt, dtype=float)
    if abs(w) < 1e-6:
        return e + v * np.sin(psi) * dt, n + v * np.cos(psi) * dt, v + 0 * dt, psi + 0 * dt, w + 0 * dt
    psi2 = psi + w * dt
    return (e + v / w * (np.cos(psi) - np.cos(psi2)),
            n + v / w * (np.sin(psi2) - np.sin(psi)), v + 0 * dt, psi2, w + 0 * dt)


def jacobian(x, dt, eps=1e-6):
    J = np.zeros((5, 5))
    base = np.array(move(x, dt))
    for j in range(5):
        d = np.zeros(5); d[j] = eps
        J[:, j] = (np.array(move(x + d, dt)) - base) / eps
    return J


def wrap(a):
    return (a + np.pi) % (2 * np.pi) - np.pi


def ekf_ctrv(rep, q_speed=0.05, q_turn=np.radians(0.05)):
    """응용 3: 등속 선회 EKF로 도는 속도까지 추정해 곡선으로 민다."""
    H = np.eye(4, 5)
    sp = rep["pos_std"][0]
    x = np.array([rep["e"][0], rep["n"][0], rep["sog"][0] * KN, np.radians(rep["cog"][0]), 0.0])
    P = np.diag([sp**2, sp**2, 1.0, np.radians(5)**2, np.radians(1)**2])
    states = []
    for k in range(len(rep["t"])):
        if k > 0:
            dt = rep["t"][k] - rep["t"][k - 1]
            J = jacobian(x, dt)
            x = np.array(move(x, dt))
            P = J @ P @ J.T + np.diag([0, 0, q_speed**2 * dt, 0, q_turn**2 * dt])
        sp = rep["pos_std"][k]
        R = np.diag([sp**2, sp**2, (rep["sog_std"][k] * KN)**2, np.radians(rep["cog_std"][k])**2])
        z = np.array([rep["e"][k], rep["n"][k], rep["sog"][k] * KN, np.radians(rep["cog"][k])])
        y = z - H @ x; y[3] = wrap(y[3])
        K = P @ H.T @ np.linalg.inv(H @ P @ H.T + R)
        x = x + K @ y; x[3] = wrap(x[3])
        P = (np.eye(5) - K @ H) @ P
        states.append(x.copy())
    states = np.array(states)

    def predict(k, tau):
        e, n, *_ = move(states[k], tau)
        return e, n
    return predict


METHODS = {"직선 DR": dead_reckoning, "칼만 필터": kalman_cv, "EKF": ekf_ctrv}
