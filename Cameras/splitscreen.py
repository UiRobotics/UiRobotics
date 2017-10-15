import numpy as np
import cv2


"""
frames is a 2 dimensional array of image data objects, column major
hight, width are the dimensions of the final frame, if not supplied the frame is not resized
"""
def split_screen(frames, height=None, width=None):
    widths = [[frame.shape[1] for frame in col] for col in frames]
    heights = [[frame.shape[0] for frame in col] for col in frames]
    col_widths = [max(col) for col in widths]
    col_heights = [sum(col) for col in heights]
    frame_width = sum(col_widths)
    frame_height = max(col_heights)
    if(width == None):
        width = frame_width
    if(height == None):
        height = frame_height
    frame = np.zeros((frame_height,frame_width,3),frames[0][0].dtype)
    row_pos = 0
    col_pos = 0
    for col in range(0,len(frames)):
        for row in range(0, len(frames[col])):
            w = frames[col][row].shape[1]
            h = frames[col][row].shape[0]
            frame[row_pos:row_pos+h, col_pos:(col_pos+w)] = frames[col][row]
            row_pos += h
        row_pos = 0
        col_pos += col_widths[col]
    return cv2.resize(frame,(width,height))
    
if(__name__=='__main__'):
    webcams = [cv2.VideoCapture(i) for i in range(0,2)]

    while(True):
        # Capture frame-by-frame
        frames = [webcam.read()[1] for webcam in webcams]
        quad = [[frames[0],frames[1]]]
        split = split_screen(quad,768,1020)
        cv2.imshow('frame', split)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    [webcam.release() for webcam in webcams]
    cv2.destroyAllWindows()

    
