from astropy.coordinates import SkyCoord, AltAz, EarthLocation
from astropy.time import Time
import astropy.units as u

# 定义天体的赤道坐标 (RA, Dec)
ra = 10.684 * u.deg  # 赤经 (RA)
dec = 41.269 * u.deg  # 赤纬 (Dec)
sky_coord = SkyCoord(ra=ra, dec=dec, frame='icrs')

# 定义观察者位置（例如北京）
location = EarthLocation(lat=39.9042 * u.deg, lon=116.4074 * u.deg, height=0 * u.m)

# 定义观察时间
observation_time = Time('2024-12-20 22:00:00')

# 转换为地平坐标
altaz = AltAz(location=location, obstime=observation_time)
sky_altaz = sky_coord.transform_to(altaz)

# 打印地平坐标
print(f"地平高度 (Altitude): {sky_altaz.alt}")
print(f"地平方位角 (Azimuth): {sky_altaz.az}")


from skyfield.api import load, Topos

# 加载星历数据
eph = load('de421.bsp')  # 星历文件
sun = eph['sun']

# 定义观察者位置（例如北京）
observer = Topos(latitude_degrees=39.9042, longitude_degrees=116.4074)

# 定义时间范围
ts = load.timescale()
start_time = ts.utc(2024, 12, 20)
end_time = ts.utc(2024, 12, 21)

# 计算太阳的升起和落下时间
t, events = eph['earth'].at(start_time).observe(sun).apparent().rise_set(observer, start_time, end_time)

# 输出结果
for ti, event in zip(t, events):
    event_str = "升起" if event == 0 else "落下"
    print(f"太阳 {event_str} 时间: {ti.utc_strftime('%Y-%m-%d %H:%M:%S')}")

from astropy.coordinates import SkyCoord
import astropy.units as u

# 定义两颗天体的赤道坐标 (RA, Dec)
star1 = SkyCoord(ra=10.684 * u.deg, dec=41.269 * u.deg, frame='icrs')  # 天体1
star2 = SkyCoord(ra=56.75 * u.deg, dec=24.116 * u.deg, frame='icrs')  # 天体2

# 计算角距离
angle = star1.separation(star2)
print(f"两天体之间的角距离: {angle}")
