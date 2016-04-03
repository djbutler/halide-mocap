#!/usr/bin/env python

import os, sys, fileinput, subprocess

def get_rotation(video):
    s = subprocess.Popen("ffmpeg -i " + video, shell=True, stderr=subprocess.PIPE).stderr.read()
    lines = [l.strip() for l in s.split('\n')]
    rotate_values = [l.split(':')[1].strip() for l in lines if 'rotate' in l]
    if len(rotate_values) > 0:
        return int(rotate_values[0])
    else:
        return 0

def get_fps(video):
    s = subprocess.Popen("ffmpeg -i " + video, shell=True, stderr=subprocess.PIPE).stderr.read()
    tbr_values = [x.strip().split(' ')[0] for x in s.split(',') if 'tbr' in x and x.strip().split(' ')[1] == 'tbr']
    if len(tbr_values) > 0:
        return float(tbr_values[0])
    else:
        print('could not find FPS - using 30')
        return 30.0

for line in fileinput.input():
    video, timestamp, frame, n_frames, outdir = line.strip().split("\t")
    n_frames = int(n_frames)
    frame = int(frame)
    fraction = (frame - 1) / get_fps(video)
    # the following is a blatent hack, but whatever
    fraction_str = ("%.4f" % fraction).split(".")[-1]
    # get rotation information for iPhone mov files
    rotation = get_rotation(video)
    if rotation == 90:
        transpose = '-vf "transpose=1"'
    elif rotation == 180:
        transpose = '-vf "transpose=2,transpose=2"'
    else:
        transpose = ''
    # construct the command    
    cmd = 'ffmpeg -ss 00:%s.%s -i %s %s -an -vframes %d -f image2 "%s/%%05d.png"' % (timestamp, fraction_str, video, transpose, n_frames, outdir)
    print(cmd)
    try:
        os.makedirs(outdir)
        p = subprocess.Popen(cmd, shell=True)
        p.wait()
    except:
        print(outdir + " already exists.")
