# DC-DC Power Stage AC Analyzer (v0.1.2b - beta)
이 프로그램은 AIMLAB(전현탁 교수님 연구실) 연구 목적으로 제작된 DCDC 컨버터 파워 스테이지 소신호 해석 도구입니다.
본 프로그램은 설계 파라미터를 기반으로 CCM/DCM 동작 모드를 자동 판별하고, 보데 플롯(Bode Plot)을 통해 제어 루프 설계에 필요한 주파수 응답 특성을 제공합니다.

## 주요 특징 (Key Features)
* **다양한 토폴로지 지원**
  * Buck 및 Boost 컨버터 해석
* **물리적 비이상성 반영**
  * 인덕터 DCR($r_L$), 커패시터 ESR($r_C$), 스위치 **On-Resistance($R_{on}$)**를 포함한 정밀 해석.
* **모드 자동 판별**
  * 부하 조건에 따른 CCM(연속 모드) 및 DCM(불연속 모드) 자동 전환 및 최적화된 수식 적용.
* **결과 데이터 내보내기**
  * 해석된 주파수 응답 데이터를 CSV 형식으로 추출하여 Excel이나 MATLAB에서 활용 가능.

## 분석 모델 및 수식(Mathematical Models)
### 1. Buck Converter
* **CCM($G_{vd}$)**
$$G_{vd}(s) = V_{in} \cdot \frac{1 + s r_C C}{1 + s \left[ \frac{L}{R} + (r_L + r_C)C \right] + s^2 LC}$$

* **DCM ($G_{vd}$):**
$$G_{vd}(s) = \frac{2V_{out}}{D_1} \cdot \frac{1-M}{2-M} \cdot \frac{1 + s r_C C}{1 + s/\omega_p}$$
(단, $\omega_p = \frac{2-M}{1-M} \cdot \frac{1}{RC}$)

### 2. Boost Converter
* **CCM($G_{vd}$)**
  * 우반평면 영점(RHP Zero)과 DCR에 의한 게인 감쇠를 반영합니다.
$$G_{vd}(s) = \frac{V_{out}}{D'} \cdot \frac{1}{1 + \frac{r_L}{R D'^2}} \cdot \frac{(1 - s\frac{L}{R D'^2})(1 + s r_C C)}{1 + s \frac{L}{R D'^2} + s^2 \frac{LC}{D'^2}}$$

* **DCM($G_{vd}$)**
$$G_{vg}(s) = M \cdot \frac{M-1}{2M-1} \cdot \frac{1 + s r_C C}{1 + s/\omega_p}$$

##  사용 방법 (Usage)
1. Design Parameters 입력
- $V_{in}$, $V_{out}$, $L$, $C$, 부하 전류($I_{out}$), 스위칭 주파수($f_{sw}$)를 입력합니다.
- 기생 성분($r_L$, $r_C$, $R_{on}$)을 입력하여 해석 정밀도를 높입니다.
2. Topology & Analysis Type 선택
- 해석하고자 하는 컨버터 타입(Buck/Boost)과 전달 함수 종류($G_{vd}$, $G_{vg}$)를 선택합니다.
3. Calculate & Plot
- 버튼을 클릭하면 하단에 계산된 파라미터(Duty, Mode, Pole/Zero 등)가 표시되며 우측에 보데 플롯이 렌더링됩니다.
4. Export CSV (Optional)
- 추가 분석이 필요한 경우 데이터를 저장합니다.

## 주의사항
* 해당 프로젝트는 Qt를 기반으로 제작된 프로젝트입니다. 실행 파일과 동일한 경로에 platforms 폴더 및 관련 DLL 파일이 존재해야 합니다.
* 본 프로그램은 AIMLAB 연구원 전용 도구이며, 상업적 이용 및 무단 복제를 금지합니다.
* Release 모드에서 발생하는 수치적 특이사항이나 버그는 wjkim에게 문의 바랍니다.
  * E-Mail : <wjikim@gmail.com>

## Information
+ Author : wjkim
+ Affiliation: [AIMLAB](https://sites.google.com/view/amp-lab/) (@CBNU)
+ Current Version : v0.1.2b (Release Date: 2026-02-19)